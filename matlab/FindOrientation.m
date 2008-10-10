function theta = FindOrientation(img, frequency, samples)
% FindOrientation finds the dominant orientation of a frequency in an
% image.
%
% Input
%   img         - the 2D image data
%   frequency   - the frequency to look for
%   samples     - the number of samples around the cirle to take
%
% Output
%   theta       - the angle at which the dominant frequency response was
%   found
global debug;
if (isempty(debug))
    debug = false;
end
if (nargin < 3)
    samples = 1440;
end

% find center coordinate of fft image
[h w] = size(img);
center = floor([h w] / 2) + 1;

% find points around frequency circle
circle = linspace(0, 2*pi, samples+1);
circle = circle(1:end-1);
pX = frequency * cos(circle) + center(2);
pY = frequency * sin(circle) + center(1);

% % do custom dft at all angles
% for i = 1:samples
%     dft(i) = abs(CustomDFT(img, circle(i), frequency));
% end

% take fft
fftimg = fftshift(fft2(img));
fftMag = abs(fftimg);

% interpolate fft magnitude around frequency of interest
% circMag = interp2(fftMag, pX, pY, 'linear');
circMag = interp2(fftMag, pX, pY, 'spline');

% find maximum magnitude
% [maxDFT, idxDFT] = max(dft);
% thetaDFT = circle(idxDFT);
[maxMag, maxIdx] = max(circMag);
thetaInterp = circle(maxIdx);

theta = thetaInterp;

if (debug)
    figure(2); clf;
    subplot(1,2,1);
    dispimg(log(fftMag));
    hold on;
    plot(pX, pY, 'r.');
    hold off;
    title('Log FFT Magnitude');
    subplot(1,2,2);
    plot(circle/pi, log(circMag)); % , circle/pi, log(dft));
    title(sprintf('Magnitude at frequency %0.3f', frequency));
    xlabel('Angle (\pi rad)');
    ylabel('Log magnitude');
%     legend({'interpolation','custom dft'});
end
