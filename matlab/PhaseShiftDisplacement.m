function [disp, shift, rotation, harmonics] = PhaseShiftDisplacement(img1, img2, featSize)
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

% window the images and compute fft
[h, w] = size(img1);
rad = round(size(img1,1)/2);
imgWind1 = WindowImage(img1, 'none', rad);
imgWind2 = WindowImage(img2, 'none', rad);
fftimg1 = fftshift(fft2(imgWind1));
fftimg2 = fftshift(fft2(imgWind2));

% if (debug)
%     figure(1); clf;
%     ShowMovie(cat(3, imgWind1, imgWind2), 2);
% end

% extract frequency components for the feature size
samps = 1440;
[fcomp1, theta1] = ExtractFrequency(fftimg1, featSize, samps); %, [-1 0 1]/2, [1/4, 1/2, 1/4]);
[fcomp2, theta2] = ExtractFrequency(fftimg2, featSize, samps); %, [-1 0 1]/2, [1/4, 1/2, 1/4]);

% find maxima around the extracted circle
[max1, imax1] = FindCircularMaxima(abs(fcomp1), theta1);
[max2, imax2] = FindCircularMaxima(abs(fcomp2), theta2);

% find harmonic frequency components along the angles of dominant frequency
[harmComp1, harmFreq1] = FindHarmonics(fftimg1, theta1(imax1(1,1)), featSize, [1:3]);
[harmComp2, harmFreq2] = FindHarmonics(fftimg2, theta1(imax1(1,1)), featSize, [1:3]);
harmonics = [harmComp1, harmComp2];

% find phase differences at all points
shiftDiff = angle(fcomp1)-angle(fcomp2);
dispDiff = shiftDiff/(2*pi) * featSize;

% phase correlation at all points
spectrum = fcomp1.*conj(fcomp2) ./ abs(fcomp1.*fcomp2);
shiftSpec = angle(spectrum);
dispSpec = shiftSpec/(2*pi) * featSize;

% find phase difference at maxima, and compute x,y translation
shift = [shiftDiff(imax1(:,1)) shiftSpec(imax1(:,1))];
basis = [cos(theta1(imax1(:,1))) sin(theta1(imax1(:,1)))];
dispBasis = dispSpec(imax1(:,1));
disp = dispBasis' * basis;
rotation = theta1(imax1(1,1));

if (debug)
    figure(3); clf;

    subplot(2,1,1);
    plot(theta1, [(log10(abs(fcomp1))) log10(abs(fcomp2))]);
    hold on;
    plot(theta1(imax1(:)), log10(max1([1 2 1 2])), 'bo', theta1(imax2(:)), log10(max2([1 2 1 2])), 'go');
    hold off;
    grid on;
    title('Freq. Mag.');
    xlabel('angle (rad)');
    legend({'img1', 'img2'});

    subplot(2,1,2);
    plot(theta1, [dispSpec]); % dispDiff]);
    grid on;
    title('Displacement');
    xlabel('angle (rad)');
    ylabel('displacement (px)');
%     legend({'spectrum','difference'});

%     subplot(3,1,3);
%     plot(harmFreq1, log10(abs(harmComp1)), 'go', harmFreq2, log10(abs(harmComp2)), 'ro');
%     title('Freq. Mag.');
%     xlabel('radius');
%     legend({'img1', 'img2'});
    
%     subplot(3,1,3);
%     plot(theta1, [shiftSpec shiftDiff]);
%     title('Phase shift');
%     xlabel('angle (rad)');
%     ylabel('shift (rad)');
%     legend({'spectrum','difference'});
%     grid on;
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

function [fftcirc, tt] = ExtractFrequency(fftimg, featSize, samples, ring, weight)
% ExtractFrequency extracts all frequency components for a given feature
% size from an image.  This is conducted in frequency space: the
% fundamental frequency for the given feature size is determined.  A circle
% is inscribed in frequency space centered at the dc term with a radius
% corresponding to the fundamental frequency.  Frequency components around
% this circle are then interpolated.
%
% Input
%   fftimg -    the fft of the image on which to operate
%   featSize -  the size of features asserted to be in the image, in pixels
%   samples -   the number of samples to extract along the circle in
%   frequency space
% Output
%   fftcirc -   the complex-valued frequency components sample around the
%   circle in frequency domain
%   tt -        the angle of each frequency component sample, in radians

global debug;

if (nargin < 4)
    ring = 0;
    weight = 1;
end

[h, w] = size(fftimg);
if (h ~= w)
    warning('image must be square');
    return;
end

if (featSize < 2 | featSize > h)
    warning('feature size must be between 2 pixels and %d pixels', h);
    return;
end

% find fundamental frequency corresponding to feature size
freq = h/featSize;

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
% replicate unit circle for different rings
circsX = repmat(unitCx, 1, length(ring));
circsY = repmat(unitCy, 1, length(ring));
bands = repmat((freq + ring), samples, 1);
px = circsX.*bands + center(2);
py = circsY.*bands + center(1);
% flatten samples
px = px(:);
py = py(:);

if (debug)
    % debug plot
    figure(2); clf;
    dispimg(log(abs(fftimg)));
    hold on; plot(px, py, 'r.'); hold off;
    pause(0.5);
end

% sample the fourier transform along the circles
fftcircR = interp2(real(fftimg), px, py);
fftcircI = interp2(imag(fftimg), px, py);

% reshape and average samples
fftcircR = reshape(fftcircR, samples, [])*weight';
fftcircI = reshape(fftcircI, samples, [])*weight';

fftcirc = complex(fftcircR, fftcircI);


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