function [ crossings ] = findZeroCrossings( data )
%FINDZEROCROSSINGS Summary of this function goes here
%  Detailed explanation goes here
yy = 1+1:size(data,1)-1;
xx = 1+1:size(data,2)-1;

crossings = zeros(size(data));
crossings(yy,xx) = (sign(data(yy-1,xx)) ~= sign(data(yy+1,xx)));
