function [ images ] = inputImages( path, filter )
%INPUTIMAGES Summary of this function goes here
%  Detailed explanation goes here

files = dir(strcat(path, filter));
images = [];
for i=1:length(files)
    img = imread(strcat(path, files(i).name));
    if (length(size(img)) > 2)
        img = rgb2gray(img);
    end;
    images = cat(1, images, struct('img', img));
end;