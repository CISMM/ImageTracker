function [ ] = circle( radius, center, style )
%CIRCLE Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 3)
    style = 'r-';
end;

step = 0.1;
theta = 0 : step : 2*pi + step;
x = cos(theta) * radius + center(2);
y = sin(theta) * radius + center(1);

plot(x, y, style);
