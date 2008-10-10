function imgPass = BandPass(img, freqRange)

[h, w] = size(img);
center = round([h w]/2) + 1;

% create frequency map mask
[xx, yy] = meshgrid(1:w, 1:h);
dist = (xx-center(2)).^2 + (yy-center(1)).^2;
freqSq = freqRange.^2;
mask = dist >= min(freqSq) & dist <= max(freqSq);

fftimg = fftshift(fft2(img));
fftimg = fftimg .* mask;
imgPass = ifft2(fftshift(fftimg));
