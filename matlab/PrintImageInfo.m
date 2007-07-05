function [ ] = PrintImageInfo( img, title )
%UNTITLED1 Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 2)
    title = 'Image';
end

disp(sprintf('%s info:', title));

disp(sprintf('\tMin:\t%f', min(min(img))));
disp(sprintf('\tMax:\t%f', max(max(img))));
disp(sprintf('\tMean:\t%f', mean(img(1:end))));
disp(sprintf('\tVar:\t%f', var(img(1:end))));
disp(sprintf('\tStd:\t%f', std(img(1:end))));
