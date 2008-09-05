function [ imgw ] = WarpImage( img, u, v, outValue )
% WarpImage Warps an input image with a deformation field d = [u, v].
% Computes the inverse warping of the input image, which avoids tearing.  A
% point x in the output image Io is given by: Io(x) = Ii(x+d(x)). Bilinear
% interpolation is used when resampling the input image.
%
% Inputs:
% img       - the input image, Ii, to warp
% u         - the x-component of the deformation field
% v         - the y-component of the deformation field
% outValue  - the value to give to pixels that map outside the input image;
% defaults to the mean of the input image.
%
% Outputs:
% imgw      - the warped output image

if (nargin < 4)
    outValue = mean(img(:));
end;

[h, w] = size(img);

[xx, yy] = meshgrid(1:w, 1:h);
imgw = interp2(img, xx+u, yy+v, 'linear', outValue);