function [ ] = ShowFeatureMovie( imgs, features, tracks, fps, filePattern )
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
% filePattern - The image filename pattern to use to save each frame
% (optional)
%
% Output:
% none

figure(gcf);
clf;

% Setup defaults
if (nargin < 5)
    filename = '';
    save = false;
else
    save = true;
end
if (nargin < 4)
    fps = 20;
end
if (nargin < 3)
    tracks = false;
end

[h, w, t] = size(imgs);
fCount = length(features);
ff = 1:fCount;

% extract feature data so we can do indexing operations
active = reshape([features.active], [], fCount); % t x N matrix
position = reshape([features.pos], t, 2, []);    % t x 2 x N matrix

% Show first frame
dispimg(imgs(:,:,1));
fidx = find(active(1,:));
hold on;
plot(squeeze(position(1, 2, fidx)), squeeze(position(1, 1, fidx)), 'ys');
hold off;
if (save)
	saveas(gcf, sprintf(filePattern, 1));
end
pause(1/fps);

% Show other frames
for i=2:t
    % show the Image
    dispimg(imgs(:,:,i));
    
    % identify the feature classes
    newFeat     = (active(i,:) == 1) & (active(i-1,:) == 0);
    contFeat    = (active(i,:) == 1) & (active(i-1,:) == 1);
    deadFeat	= (active(i,:) == 0) & (active(i-1,:) == 1);
    
    hold on; % keep the image up while drawing features
    % Show each feature's history, if necessary
    if (tracks)
        actIdx = find(contFeat);
        for featIdx = actIdx
            frameIdx = find(active(:, featIdx)); % feature track history
            % crop history to this frame
            frameIdx = frameIdx.*(frameIdx <= i); 
            frameIdx = frameIdx(frameIdx ~= 0);
            % display track
            plot(position(frameIdx, 2, featIdx), ...
                position(frameIdx, 1, featIdx), 'g-');
        end
    end
    
    % display the features, color coded
    plot(squeeze(position(i, 2, newFeat)),  squeeze(position(i, 1, newFeat)),  'yx');
    plot(squeeze(position(i, 2, contFeat)), squeeze(position(i, 1, contFeat)), 'gs');
    plot(squeeze(position(i, 2, deadFeat)), squeeze(position(i, 1, deadFeat)), 'ro');

    hold off; % done drawing on the figure

    % Grab frame, if we're saving the movie
    if (save)
        saveas(gcf, sprintf(filePattern, i));
    end
    pause(1/fps);
end

% Save the video file
if (save)
%     display(sprintf('Saving avi file to %s', filename));
%     for i = 1:length(mov)
%         imwrite(mov(i).cdata, sprintf(filename, i), 'Compression', 'None');
%     end
%     movie2avi(mov,filename,'FPS',fps,'Quality',90);
%     movie2avi(mov,filename,'FPS',fps,'Compression','None');
end