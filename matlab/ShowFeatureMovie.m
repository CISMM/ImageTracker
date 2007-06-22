function [ ] = ShowFeatureMovie( imgs, feats, fps, filename )
% ShowFeatureMovie(imgs, feats) displays an image sequence along with
% tracked features.
%
% Features are color-coded with the following scheme:
% green  - a feature newly detected in the current frame
% yellow - a feature successfully tracked in the current frame
% red    - a feature whose tracking fails in the current frame
% 
% The play rate can be controlled with the fps variable.  This is mainly a
% suggestion of how long to wait (1/fps) after displaying one frame and
% before displaying the next one; the frame rate could drop lower if
% plotting many features.
%
% The function enables saving an AVI to the specified filename.  If no
% filename is specified, no movie will be saved.  The target frame rate is
% used for this video.  The default codec with a quality level of 90 is
% used.
%
% Input (default)
% imgs        - The image sequence to display
% feats       - The features tracked in the image sequence
% fps         - The target display frame rate (20)
% filename    - The file name to use to save an AVI movie of the feature
% tracking
%
% Output:
% none

% Setup defaults
if (nargin < 4)
    filename = '';
    save = false;
else
    save = true;
end;
if (nargin < 3)
    fps = 20;
end;

[hi,wi,ti] = size(imgs);
[hf,wf,tf] = size(feats);
ff = 1:hf;

% Show first frame
dispimg(imgs(:,:,1));
fidx = find(feats(:,4,1));
hold on;
plot(feats(fidx,2,1), feats(fidx,1,1), 'gs');
hold off;
if (save)
    mov(1) = getframe;
end;
pause(1/fps);

% Show other frames
for i=2:min(ti,tf)
    % show the Image
    dispimg(imgs(:,:,i));
    
    % identify the feature classes
    newFeat(ff)  = (feats(ff,4,i) == 1).*(feats(ff,4,i-1)==0);
    contFeat(ff) = (feats(ff,4,i) == 1).*(feats(ff,4,i-1)==1);
    deadFeat(ff) = (feats(ff,4,i) == 0).*(feats(ff,4,i-1)==1);
    newIdx  = find(newFeat);
    contIdx = find(contFeat);
    deadIdx = find(deadFeat);
    
    % display the features, color coded
    hold on;
    plot(feats(newIdx, 2,i), feats( newIdx,1,i), 'gs');
    plot(feats(contIdx,2,i), feats(contIdx,1,i), 'ys');
    plot(feats(deadIdx,2,i), feats(deadIdx,1,i), 'rs');
    hold off;
    
    % Grab frame, if we're saving the movie
    if (save)
        mov(i) = getframe;
    end;
    pause(1/fps);
end;

% Save the video file
if (save)
    display(sprintf('Saving avi file to %s', filename));
    movie2avi(mov,filename,'FPS',fps,'Quality',90);
%     movie2avi(mov,filename,'FPS',fps,'Compression','None');
end;