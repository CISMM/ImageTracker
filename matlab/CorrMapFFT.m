function [ map ] = CorrMapFFT( img, template )
% CORRMAPFFT Summary of this function goes here
%   Detailed explanation goes here

[h,w] = size(img);

% normalize
img = img - mean(img(:));
img = img/norm(img(:));
template = template - mean(template(:));
template = template/norm(template(:));

% pad template
t = zeros(size(img));
t(1:size(template,1), 1:size(template,2)) = template;
template = t;

% Multiply in fourier space
Fimg = fft2(img);
Ftemp = fft2(template);

mult = Fimg.*Ftemp;
map = real(ifft2(mult));
% map = reshape(map, h, w);