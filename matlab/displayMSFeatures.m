function [ ] = displayMSFeatures( img, features, style )
%DISPLAYFEATURES Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 3)
    style = 'r-';
end;

count = size(features,1);
sRatio = 3;

imshow(img);
colormap gray;
axis image;
hold on;

for i=1:count
    center = features(i,1:2);
    if (size(features,2) > 3)
        A = reshape(features(i,3:6),2,2);
        theta = features(i,8);
        R = [cos(theta) sin(theta); -sin(theta) cos(theta)];
        A = inv(R) * A;
        s = features(i,7);
    else
        A = eye(2);
        s = features(i,3);
    end;
    
    ellipse(s * sRatio, center, inv(A), style);
    plot(center(2),center(1),'r.');
end;

hold off;