function [ map ] = CorrMapFFT( img, template )
% CORRMAPFFT Summary of this function goes here
%   Detailed explanation goes here

[h,w] = size(img);

% pad template
% t = zeros(size(img));
% t(1:size(template,1), 1:size(template,2)) = template;
% template = fftshift(t);

pad = size(img)-size(template);
tpad = padarray(template, ceil(pad/2));
tpad = tpad(1:size(img,1), 1:size(img,2));

% Multiply in fourier space
Fimg = fft2(img);
Ftemp = fft2(tpad);

mult = Fimg.*conj(Ftemp);
map = real(fftshift(ifft2(mult)));
