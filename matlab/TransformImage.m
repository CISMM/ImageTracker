function [ imgT, dx, dy ] = TransformImage( img, outSize, offset, tform )
% TransformImage transforms an image with a given tranform matrix.
%
% Input:
%   img         - the input image
%   outSize     - the size of the output images
%   offset      - the position of the upper left corner of the output images
%   tform       - the transform to apply

        
% find original pixel locations
xx=(1:outSize(2)) + offset(2);
yy=(1:outSize(1)) + offset(1);
[xx,yy] = meshgrid(xx,yy);
hh=ones(outSize(1),outSize(2));
pos = [reshape(xx, 1, []); reshape(yy, 1, []); reshape(hh, 1, [])];

% apply transform to point locations
% inverse transform--this is for tear-free image warping
pos = inv(tform) * pos;

% reshape transformed pixel locations
posX = reshape(pos(1,:), outSize(1), outSize(2));
posY = reshape(pos(2,:), outSize(1), outSize(2));

% interpolate using transformed point locations
imgT = interp2(img, posX, posY, 'linear', 0);
dx = xx - posX;
dy = yy - posY;
