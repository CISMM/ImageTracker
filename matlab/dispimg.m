function [] = dispimg(img, cmap)
% dispimg(img, color) Display an image and scale appropriately.
%
% Display the given image with the chosen color map.
% Inputs (default):
% img         - The input image
% cmap        - The colormap to use (gray)
%
% Outputs:
% none

if (nargin < 2)
    cmap = 'gray';
end;

imagesc(img);
axis image;
colormap(cmap);
