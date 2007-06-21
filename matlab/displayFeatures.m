function [ ] = displayFeatures( img, feat, style )
% DisplayFeatures(img, feat, style) Displays feature points on an image.
% Displays the given image and draws feature points using the given style.
%
% Inputs (default)
% img      - The image to display
% feat     - The features points to draw
% style    - The plot drawing style to use ('r+')
%
% Outputs
% none

if (nargin < 3)
    style = 'r+';
end;

dispimg(img);
hold on;
plot(feat(:,2), feat(:,1), style);
hold off;