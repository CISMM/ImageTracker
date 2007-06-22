function [ ] = ShowMovie( frames, fps )
% ShowMovie(frames, fps) Displays an image sequence in a figure window at
% the requested frame rate (fps, default is 30).

[h, w, l] = size(frames);

if (nargin < 2)
    fps = 30;
end;

for i = 1:l
    dispimg(frames(:,:,i));
    pause((1/fps));
end;
