function [ ] = ShowMovie( frames, fps, filename )
% ShowMovie(frames, fps) Displays an image sequence in a figure window at
% the requested frame rate (fps, default is 30).

% force the current frame to have focus
figure(gcf);

if (nargin < 3)
    filename = '';
    save = false;
else
    save = true;
end;

if (nargin < 2)
    fps = 30;
end;

[h, w, l] = size(frames);

for i = 1:l
    msg = sprintf('frame %d', i);
    dispimg(frames(:,:,i), msg);    
    pause((1/fps));

    if (save)
        mov(i) = getframe;
    end;
end;

if (save)
    display(sprintf('Saving avi file to %s', filename));
    movie2avi(mov,filename,'FPS',fps,'Quality',90);
%     movie2avi(mov,filename,'FPS',fps,'Compression','None');
end;