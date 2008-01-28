function [ kernel, radius ] = GaussianKernel1D( scale, deriv, width )
% GaussianKernel1D( sigma, deriv, width ) Creates a centered Gaussian
% kernel of the specified derivative order and the specified scale. The
% width parameter specifies how many sigma from the center the kernel
% should extend.
if (nargin < 3)
    width = 3;
end;

% sigma = sqrt(scale/(2*pi));
sigma = scale;

radius = ceil(sigma) * width;
x = -radius:1:radius;
derivs(1,1:length(x)) = 1;
derivs(2,1:length(x)) = -(x/(sigma^2));
derivs(3,1:length(x)) = (x.^2 - (sigma^2))/(sigma^4);
derivs(4,1:length(x)) = -(x.^3 - 3 * sigma^2 * x)/(sigma^6);
derivs(5,1:length(x)) = (x.^4 - 6 * sigma^2 * x.^2 + 3 * sigma^4)/(sigma^8);
kernel = (1/(sigma*sqrt(2*pi))) * exp(-x.^2/(2*sigma^2));
kernel = kernel .* derivs(deriv+1,:);

