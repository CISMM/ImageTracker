function [disp, theta] = FrequencyDisplacement(img1, img2, fundamental, theta, taylor)

if (nargin < 5)
    taylor = false;
end

global debug;
if (isempty(debug))
    debug = false;
end

[h, w] = size(img1);

% check image dimensions
if (h ~= w)
    warning('Images must be square: %d ~= %d', h, w);
    disp = [0 0]; theta = 0;
    return;
end
if ([h, w] ~= size(img2))
    warning('Images must be the same size.');
    disp = [0 0]; theta = 0;
    return;
end
if (fundamental < 2 | fundamental > h)
    warning('Fundamental wavelength must be between 2 and %d pixels', h);
    disp = [0 0]; theta = 0;
    return;
end

% harmonic wavelengths and frequencies
wavelength = fundamental ./ [1 2 3 4 5 6];
frequency = h ./ wavelength;

% window images
rad = round(h / 2);
imgWind1 = WindowImage(img1, 'none', rad);
imgWind2 = WindowImage(img2, 'none', rad);

% find orientation
if (nargin < 4)
    theta = FindOrientation(imgWind1, frequency(1));
end

% Frequency-specific FFTs
fftU1 = CustomDFT(imgWind1, theta, frequency);
fftU1 = diag(fftU1);
fftV1 = CustomDFT(imgWind1, theta + pi/2, frequency);
fftV1 = diag(fftV1);
fftU2 = CustomDFT(imgWind2, theta, frequency);
fftU2 = diag(fftU2);
fftV2 = CustomDFT(imgWind2, theta + pi/2, frequency);
fftV2 = diag(fftV2);

% Phase correlation
% specU = fftU1.*conj(fftU2) ./ abs(fftU1.*fftU2);
% specV = fftV1.*conj(fftV2) ./ abs(fftV1.*fftV2);
% phaseUV = [angle(specU) angle(specV)];

% Measure phase shift
if (taylor)
    dpt1 = TaylorDPT(img1, wavelength, theta);
    dpt2 = TaylorDPT(img2, wavelength, theta);
    phaseUV = angle(dpt1) - angle(dpt2);
else
    shiftU = angle(fftU1) - angle(fftU2);
    shiftV = angle(fftV1) - angle(fftV2);
    phaseUV = [shiftU shiftV];
end;

% Enforce maximum displacement of half wavelength in either direction
wrapIdx = phaseUV < -pi;
phaseUV(wrapIdx) = phaseUV(wrapIdx) + 2*pi;
wrapIdx = phaseUV > pi;
phaseUV(wrapIdx) = phaseUV(wrapIdx) - 2*pi;

% determine displacement in XY
UVtoXY = [cos(theta) sin(theta); -sin(theta) cos(theta)];

if (taylor)
    % in this case, the phase shifts are known in UV for the wavelengths of
    % the pattern
    dispUV = phaseUV / (2*pi) .* repmat(wavelength', 1, 2);
    dispXY = dispUV * UVtoXY;
else
    % in this case, the phase shifts are known in UV for rotated
    % wavelengths of the pattern
    phaseXY = phaseUV * UVtoXY;
    dispXY = phaseXY / (2*pi) .* repmat(wavelength', 1, 2);
end

% robust displacement estimation
modelParams = 1;
pointError = 0.4;
inlierPct = 0.50;
certainty = 0.999;
dispX = Ransac(dispXY(:,1)', @CreateModel, @ErrorModel, modelParams, pointError, inlierPct, certainty)';
dispY = Ransac(dispXY(:,2)', @CreateModel, @ErrorModel, modelParams, pointError, inlierPct, certainty)';
disp = [dispX dispY];
% disp = median(dispXY(2:5,:));

if (debug)
    fprintf('\t    X\t\t    Y\t\t    W\n');
    for i = 1:size(phaseXY,1)
        fprintf('\t%8.3f\t%8.3f\t%8.3f\n', dispXY(i,:), wavelength(i));
    end
    fprintf('Model:  %8.3f\t%8.3f\n', disp);
end;


function cdft = CustomDFT(img, theta, frequency)
% CustomDFT computes the Fourier transform at specific frequencies and
% orientations.
%
% Input
%   img         - the 2D image data
%   theta       - the orientation of the frequency
%   frequency   - the set of frequencies at which to compute the FT
%
% Output
%   cdft        - the Fourier transform components requested
[h, w] = size(img);

% range of image
x = (0:h-1);

% find frequency components for given orientation
fx = frequency(:) * cos(theta);     % freq x 1
fy = frequency(:) * sin(theta);
% fprintf('fs: %0.3f => %0.3f x, %0.3f y\n', frequency(1), fx(1), fy(1));

Mx = cos(2*pi/h * (fx * x)) - i * sin(2*pi/h * (fx * x));
My = cos(2*pi/h * (fy * x)) - i * sin(2*pi/h * (fy * x));

% NOTE: non-conjugate transpose is needed; we are just rotating
% the Fourier matrix to get the x components.
cdft = My * img * transpose(Mx);


function model = CreateModel(data)
model = mean(data, 2);

function error = ErrorModel(data, model)
N = size(data,2);
models = repmat(model, 1, N);
error = sqrt(sum((data - models).^2, 1));