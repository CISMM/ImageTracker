function [ output_args ] = createFeatureMosaic( img, features, mWidth )
%CREATEFEATUREMOSAIC Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 3)
    mWidth = 10;
end;

figure;
colormap gray;

count = size(features, 1);
mHeight = ceil(count / mWidth);
windRatio = 5;
circRatio = 3;

A = eye(2);

for i = 1:count
    center = features(i,1:2);
    s = features(i,3);
    wWidth = ceil(s * windRatio);
    wind = warppatch2(img,center',A,wWidth);
    
    xw = wWidth+1; yw = wWidth+1;
    
    subplot(mHeight, mWidth, i);
    imshow(wind);
    axis image;
    hold on;
    circle(s * circRatio, [yw, xw]);
    hold off;
    
end;
