function [ fData ] = resizeGauss1D( data, sigma )
%RESIZEGAUSS1D Summary of this function goes here
%  Detailed explanation goes here

g = GaussianKernel1D(sigma, 0);
half = floor(length(g)/2);
fData = conv(data(:,2), g);
fData = fData(half+1:end-half);
skip = 1:sigma:length(fData);

fData = [data(skip,1), fData(skip)];