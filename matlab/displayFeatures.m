function [ ] = displayFeatures( img, feat, style )
%DISPLAYFEATURES Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 3)
    style = 'r+';
end;

imagesc(img);
hold on;
plot(feat(:,2), feat(:,1), style);
hold off;