function [ gImg ] = createGaussPyramid( img, levels, scale )
% createGaussPyramid Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 3)
    scale = sqrt(sqrt(2));
end;
if (nargin < 2)
    levels = 10;
end;

gauss = fspecial('gaussian', ceil(2 * scale * 3), scale);
gImg(1,:,:) = img;
for i = 1:levels
    gImg(i+1,:,:) = conv2(squeeze(gImg(i,:,:)), gauss, 'same');
end;

    