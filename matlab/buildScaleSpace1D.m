function [ Gxdata ] = buildScaleSpace1D( data, minSigma, maxSigma, step )
%BUILDSCALESPACE1D Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 4)
    step = sqrt(sqrt(2));
end;

Gdata = [];
Gxdata = [];
for s = minSigma:step:maxSigma
%     g = GaussianKernel1D(s, 0);
    dg = GaussianKernel1D(s, 1);
%     gData = conv(data', g);
    dgData = conv(data', dg);
%     half = floor(length(g)/2);
%     Gdata(end+1,:) = gData(half + 1:end-half);
    half = floor(length(dg)/2);
    Gxdata(end+1,:) = dgData(half + 1:end-half);
end;