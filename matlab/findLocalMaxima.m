function [ maxima ] = findLocalMaxima( data )
%FINDLOCALMAXIMA Summary of this function goes here
%  Detailed explanation goes here

yy = 1+1:size(data,1)-1;
xx = 1+1:size(data,2)-1;

maxima = zeros(size(data));
maxima(yy,xx) = (data(yy,xx) >= data(yy-1,xx)) & (data(yy,xx) >= data(yy+1,xx));
