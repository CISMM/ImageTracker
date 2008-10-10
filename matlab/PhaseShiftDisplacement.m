function [disp, theta, harmonics] = PhaseShiftDisplacement(img1, img2, featSize, theta)
% PhaseShiftDisplacement finds the displacement between two images
% considering the frequencies represented by a particular feature size.
%
% Input
%   img1 -      the first of two image to consider
%   img2 -      the second image
%   featSize -  the size of features asserted to be present in both images,
%   in pixels
% Output
%   disp -      the displacement (x,y) found between the two images, in
%   pixels
%   shift -     the phase shift found between the two images

global debug;
if (isempty(debug))
    debug = false;
end

[h, w] = size(img1);
% frequency and harmonics
if (featSize < 2 | featSize > h)
    warning('feature size must be between 2 pixels and %d pixels', h);
    return;
end
freqs = h / featSize * [1 2 3 4 5 6];

% window the images and compute fft
rad = round(size(img1,1)/2);
imgWind1 = WindowImage(img1, 'none', rad);
imgWind2 = WindowImage(img2, 'none', rad);
fftimg1 = fftshift(fft2(imgWind1));
fftimg2 = fftshift(fft2(imgWind2));

% extract frequency components for the feature size
if (nargin < 4)
    samps = 720;
    [magCirc1, theta1] = ExtractFrequency(fftimg1, freqs(1), samps);
    [magCirc2, theta2] = ExtractFrequency(fftimg2, freqs(1), samps);

    % find maxima around the extracted circle
    [max1, imax1] = FindCircularMaxima(magCirc1, theta1);
    [max2, imax2] = FindCircularMaxima(magCirc2, theta2);
    theta = theta1(imax1(:,1));
end

% extract all components along the angles of dominant frequency
[magPhase1, frequency1] = ExtractAngle(fftimg1, theta, freqs);
[magPhase2, frequency2] = ExtractAngle(fftimg2, theta, freqs);

% find phase differences at all frequencies along orientation lines
wavelength = h ./ frequency1;
[dmin, fundamentalIdx] = min((wavelength - featSize).^2);
phaseShift = magPhase1(:,3:4) - magPhase2(:,3:4);           % N x 2

% correct for > pi displacements
wrapIdx = phaseShift > pi;
phaseShift(wrapIdx) = phaseShift(wrapIdx) - 2 * pi;
wrapIdx = phaseShift < -pi;
phaseShift(wrapIdx) = phaseShift(wrapIdx) + 2 * pi;

% find displacement along orientation directions
dispDiff = phaseShift/(2*pi) .* repmat(wavelength, 1, 2);   % N x 2
% fprintf('At %0.3f px, phase => %0.3f\t%0.3f\n', wavelength(fundamentalIdx), phaseShift(fundamentalIdx, :));

% compute x,y translation
basis = [cos(theta) sin(theta)];  % 2 x 2
dispBasis = dispDiff * basis;                               % N x 2
disp = dispBasis(fundamentalIdx, :);

% find harmonic frequency components along the angles of dominant frequency
[harmComp1, harmFreq1] = FindHarmonics(fftimg1, theta(1), featSize, [1:3]);
[harmComp2, harmFreq2] = FindHarmonics(fftimg2, theta(1), featSize, [1:3]);
harmonics = [harmComp1, harmComp2];

% phase correlation at all points
% spectrum = fcomp1.*conj(fcomp2) ./ abs(fcomp1.*fcomp2);
% shiftSpec = angle(spectrum);
% dispSpec = shiftSpec/(2*pi) * featSize;

% find phase difference at maxima, and compute x,y translation
% shift = [shiftDiff(imax1(:,1)) shiftSpec(imax1(:,1))];
% basis = [cos(theta1(imax1(:,1))) sin(theta1(imax1(:,1)))];
% dispBasis = dispSpec(imax1(:,1));
% disp = dispBasis' * basis;
% rotation = theta1(imax1(1,1));

if (debug)
    figure(3); clf;
    subplot(2,1,1);
    plot(wavelength, phaseShift);
    subplot(2,1,2);
    plot(wavelength, dispBasis);
end

function [maxVal, maxIdx] = FindCircularMaxima(data, angles)
% FindCircularMaxima finds two maximum values for data distributed around a
% circle.  This function assumes that there are two matched pairs of maxima
% around the circle (four maxima total), that each pair is mirrored--
% separated by pi radians--and that the pairs of maxima are
% orthogonal--separated by pi/2 radians.

% Find the locations of maxima on the circle
ii = 1:length(data);
dataP1 = data([ii(end) ii(1:end-1)]);
dataM1 = data([ii(2:end) ii(1)]);
maxPos = find(data(ii) > dataP1(ii) & data(ii) > dataM1(ii));

% Sort the maximum positions by descending magnitude
[sortVal, sortIdx] = sort(data(maxPos), 'descend');

% Grab largest maximum and find the maximum that is closest to pi radians
% away.
maxVal(1) = sortVal(1);
shift = mod(angles(maxPos(sortIdx(:))) - (angles(maxPos(sortIdx(1))) + pi), 2*pi);
[minVal, minIdx] = min(shift);
maxIdx(1,:) = [maxPos(sortIdx(1)) maxPos(sortIdx(minIdx))];

% Find the maximum that is closest to pi/2 radians away.
shift = mod(angles(maxPos(sortIdx(:))) - (angles(maxPos(sortIdx(1))) + pi/2), 2*pi);
[minVal, minIdx] = min(shift);
maxVal(2) = sortVal(minIdx);
maxIdx(2,1) = maxPos(sortIdx(minIdx));
% Find the maximum that is closest to pi radians from this second maximum.
shift = mod(angles(maxPos(sortIdx(:))) - (angles(maxPos(sortIdx(minIdx))) + pi), 2*pi);
[minVal, minIdx] = min(shift);
maxIdx(2,2) = maxPos(sortIdx(minIdx));

% reorder the maximum pairs
maxIdx(1,:) = [min(maxIdx(1,:)) max(maxIdx(1,:))];
maxIdx(2,:) = [min(maxIdx(2,:)) max(maxIdx(2,:))];

function [fftcirc, tt] = ExtractFrequency(fftimg, freq, samples)
% ExtractFrequency extracts all frequency components for a given feature
% size from an image.  This is conducted in frequency space: the
% fundamental frequency for the given feature size is determined.  A circle
% is inscribed in frequency space centered at the dc term with a radius
% corresponding to the fundamental frequency.  Frequency components around
% this circle are then interpolated.
%
% Input
%   fftimg -    the fft of the image on which to operate
%   freq -      the frequency asserted to be in the image
%   samples -   the number of samples to extract along the circle in
%   frequency space
% Output
%   fftcirc -   the complex-valued frequency components sample around the
%   circle in frequency domain
%   tt -        the angle of each frequency component sample, in radians

global debug;

[h, w] = size(fftimg);
if (h ~= w)
    warning('image must be square');
    return;
end

% choose a good sampling rate, if not provided explicitly
if (nargin < 3)
    samples = ceil(30*freq);
end

% find center of circle
center = floor([h w]/2) + 1;

% find sampling points on the circle
tt = linspace(0,2*pi,samples+1)';
tt = tt(1:end-1); % last sample is a repeat
unitCx = cos(tt);
unitCy = sin(tt);
% find points on frequency-specific circle
px = unitCx*freq + center(2);
py = unitCy*freq + center(1);

% sample the fourier transform along the circles
fftcirc = interp2(abs(fftimg), px, py, 'linear');

if (debug)
    % debug plot
    figure(2); clf;
    subplot(2,1,1);
    dispimg(log(abs(fftimg)));
    hold on; plot(px, py, 'r.'); hold off;
    subplot(2,1,2);
    plot(tt, log(fftcirc));
	drawnow;
    pause(0.5);
end

function [magPhase, radius] = ExtractAngle(fftimg, theta, freqs)
% ExtractAngle extracts all frequency components in a line oriented by the
% given angles
global debug;

[h, w] = size(fftimg);
center = floor([h w] / 2) + 1;
radius = (1:0.02:1.25*freqs(end))';          % N x 1
posX = center(2) + radius * cos(theta(:)'); % N x t
posY = center(1) + radius * sin(theta(:)'); % N x t
compR = interp2(real(fftimg), posX, posY, 'linear');
compI = interp2(imag(fftimg), posX, posY, 'linear');
compM = interp2(abs(fftimg), posX, posY, 'linear');
compP = interp2(angle(fftimg), posX, posY, 'linear');
magPhase = [compM compP];

if (debug)
    figure(2); clf;
    subplot(1,2,1);
    dispimg(log(abs(fftimg)));
    hold on; plot(posX(:,1), posY(:,1), 'r-', posX(:,2), posY(:,2), 'g-'); hold off;
    subplot(1,2,2);
    hold on;
    plot(radius, log(compM(:,1)), 'r-', radius, log(compM(:,2)), 'g-');
    plot(radius, compP(:,1), 'r-', radius, compP(:,2), 'g-');
    plot(freqs, zeros(size(freqs)), 'b+');
    hold off;
    drawnow;
    pause(0.5);
end

function [harmComp, freqs] = FindHarmonics(fftimg, theta, featSize, harmonics)
% ExtractAngle extracts all frequency components in a line oriented by the
% given angle.
[h, w] = size(fftimg);

freqs = h ./ (featSize ./ harmonics);
% radii = linspace(0, freqs(end), 400);

mid = floor([h w]/2);
center = mid + 1;

posX = center(1) + freqs * cos(theta);
posY = center(2) + freqs * sin(theta);
harmCompR = interp2(real(fftimg), posX, posY);
harmCompI = interp2(imag(fftimg), posX, posY);
harmComp = complex(harmCompR, harmCompI);