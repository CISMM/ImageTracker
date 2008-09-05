function [ img ] = RandomSplat( imgSize, thresh, kernel )
% RandomSplat creates an image of a kernel splatted at random points.
%
% Input
%   imgSize -   the target image size
%   thresh -    controls density of splats; higher threshold lowers density
%   kernel -    the image of splats to create
% Output
%   img -       the random image

img = zeros(imgSize);
rKern = floor(size(kernel) / 2);

% Select a random set of points
rimg = rand(imgSize) > thresh;
rimg(1:rKern(1),:) = 0;
rimg(imgSize(1)-rKern(1):imgSize(1),:) = 0;
rimg(:,1:rKern(2)) = 0;
rimg(:,imgSize(2)-rKern(2):imgSize(2)) = 0;

[i,j] = find(rimg);
for p = 1:length(i)
    x = j(p);
    y = i(p);
    img(y-rKern(1):y+rKern(1),x-rKern(2):x+rKern(2)) = img(y-rKern(1):y+rKern(1),x-rKern(2):x+rKern(2)) + kernel;
end;
