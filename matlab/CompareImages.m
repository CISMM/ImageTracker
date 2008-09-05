function [rmse, psnr, sqErr] = CompareImages(img1, img2, bits)
% CompareImages computes image comparison metrics for two images: the peak
% signal to noise ratio and root mean squared error.

if (nargin < 3)
    bits = 8;
end

[h, w] = size(img1);
sqErr = (img2-img1).^2;
rmse = sqrt(1/(h*w) * sum(sqErr(:)));

maximum = 2^bits-1;
psnr = 20 * log10(maximum/rmse);
