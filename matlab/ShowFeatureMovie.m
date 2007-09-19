function [ ] = ShowFeatureMovie( imgs, feats, tracks, fps, filename )
% ShowFeatureMovie(imgs, feats) displays an image sequence along with
% tracked features.
%
% Features are color-coded with the following scheme:
% green  - a feature newly detected in the current frame
% yellow - a feature successfully tracked in the current frame
% red    - a feature whose tracking fails in the current frame
%
% The tracks parameter determines whether to display each feature's
% history as a set of connected blue dots.
% 
% The play rate can be controlled with the fps variable.  This is mainly a
% suggestion of how long to wait (1/fps) after displaying one frame and
% before displaying the next one; the frame rate could drop lower if
% plotting many features or when displaying the feature tracks.
%
% The function enables saving an AVI to the specified filename.  If no
% filename is specified, no movie will be saved.  The target frame rate is
% used for this video.  The default codec with a quality level of 90 is
% used.
%
% Input (default)
% imgs        - The image sequence to display
% feats       - The features tracked in the image sequence
% tracks      - A flag indicating whether to display the feature tracks
% fps         - The target display frame rate (20)
% filename    - The file name to use to save an AVI movie of the feature
% tracking
%
% Output:
% none

% Setup defaults
if (nargin < 5)
    filename = '';
    save = false;
else
    save = true;
end;
if (nargin < 4)
    fps = 20;
end;
if (nargin < 3)
    tracks = false;
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
    
    hold on; % keep the image up while drawing features

    % Show each feature's history, if necessary
    if (tracks)
        fidx = find(feats(:,4,i)); % valid features
        for f = 1:length(fidx)
            tidx = find(feats(fidx(f),4,:)); % feature track history
            % crop history to this frame
            tidx = tidx.*(tidx <= i); 
            tt = find(tidx);
            % display track
            plot(squeeze(feats(fidx(f),2,tidx(tt))), squeeze(feats(fidx(f),1,tidx(tt))), 'y-');
        end;
    end;
    
    % display the features, color coded
    plot(feats(newIdx, 2,i), feats( newIdx,1,i), 'gs');
    plot(feats(contIdx,2,i), feats(contIdx,1,i), 'ys');
    plot(feats(deadIdx,2,i), feats(deadIdx,1,i), 'rs');

    hold off; % done drawing on the figure

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