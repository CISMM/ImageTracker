function [] = dispimg(img, caption, cmap)
% dispimg(img, color) Display an image and scale appropriately.
%
% Display the given image with the chosen color map.
% Inputs (default):
% img         - The input image
% caption     - A caption to display with the image
% cmap        - The colormap to use (gray)
%
% Outputs:
% none

if (nargin < 3)
    cmap = 'gray';
end;
if (nargin < 2)
    caption = '';
end;


imagesc(img);
axis image;
colormap(cmap);
title(caption);
