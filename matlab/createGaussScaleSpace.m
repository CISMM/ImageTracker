function [ space ] = createGaussScaleSpace( data, deriv, initScale, finScale, step )
%CREATEGAUSSSCALESPACE Summary of this function goes here
%  Detailed explanation goes here

scales = initScale:step:finScale;
for i = 1:length(scales)
    scale = scales(i);
    g = GaussianKernel1D(scale, deriv);
    half = floor(length(g)/2);
    fData = conv(data, g);
    fData = fData(half+1:end-half);
    space(:,i) = fData;
end;
    