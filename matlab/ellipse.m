function [ ] = ellipse( radius, center, aff, style )
%CIRCLE Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 4)
    style = 'r-';
end;

step = 0.1;
theta = 0 : step : 2*pi + step;
circ(1,:) = cos(theta) * radius;
circ(2,:) = sin(theta) * radius;

% Affine transform is a 2x2.
circ = aff * circ;
for i=1:length(circ)
    circ(:,i) = circ(:,i) + [center(2);center(1)];
end;

plot(circ(1,:), circ(2,:), style);
