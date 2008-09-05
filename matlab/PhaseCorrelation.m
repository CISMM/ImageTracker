function [dt, corrmap] = PhaseCorrelation(img1, img2, featSize)
% PhaseCorrelation computes the translation between two images of the same
% scene using the Fourier shift theorem.
% Computes the cross-power spectrum of the two supplied images, and
% determiens the phase shift between them.  The inverse Fourier transform
% of the cross-power spectrum yields an approximate Dirac impulse at the
% displacement that best registers the two input images.

% Apply windowing to images
rad = round(size(img1,1)/2);
img1pad = WindowImage(img1, 'none', rad);
img2pad = WindowImage(img2, 'none', rad);

ShowMovie(cat(3, img1pad, img2pad), 2);

% compute Fourier transforms
fimg1 = fft2(img1pad);
fimg2 = fft2(img2pad);

if (nargin >= 3) % select for feature size
    % find distance from corners
    [h, w] = size(fimg1);
    [xx,yy] = meshgrid(1:w, 1:h);
    xd = min(xx-1, w-xx+1);
    yd = min(yy-1, h-yy+1);
    dist = sqrt(xd.^2 + yd.^2);
    pad = 1.1;
    freqCutoffH = ceil(pad*h/featSize);
    freqCutoffL = floor((1/pad)*h/featSize);
    distPass = dist <= freqCutoffH & dist >= freqCutoffL;
    distPass(1,1) = 0; % dc term
    fimg1 = fimg1.*distPass;
    fimg2 = fimg2.*distPass;
end

% compute cross-power spectrum
spectrum = fimg1.*conj(fimg2);
normal = abs(fimg1.*fimg2);
thresh = 0;
% avoid divide by nearly zero
notZero = (normal - thresh) > 0;
spectrum(notZero) = spectrum(notZero) ./ normal(notZero);
spectrum(~notZero) = 0;

% inverse transform gives correlation map
corrmap = real(ifft2(spectrum));

% find the peak on the map
[y, x] = find(corrmap == max(corrmap(:)));
peak = [x, y];

% interpret translation
h = size(img1,1);
mid = floor(h/2);
dt = peak-1;
dt(dt>mid) = -mod(h-dt(dt>mid), h);