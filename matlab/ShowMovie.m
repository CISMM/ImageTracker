function [ ] = ShowMovie( frames, fps )
%SHOWMOVIE Summary of this function goes here
%   Detailed explanation goes here

[h, w, l] = size(frames);

if (nargin < 2)
    fps = 30;
end;

for i = 1:l
    dispimg(frames(:,:,i));
    pause((1/fps));
end;
