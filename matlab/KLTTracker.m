function [ features ] = KLTTracker( imgs, sigmaS, sigmaT, count, featRadius, featFrame, windRadius, iters )
% KLTTracker( imgs, sigmaS, sigmaT, count, featRadius, featFrame ) Tracks
% features in an image sequence.
%
% An implemntation of the KLT (Kanade, Lucas, Tomasi) tracker.  Uses a
% Harris interest feature detector to find features to track.  The set of
% features is tracked for each frame using a closed-form linear equation
% that presumes features have a constant appearance over time.  At each
% frame, tracking error for each feature is established by comparing the
% current feature patch to the initially detected feature patch.  Features
% that fail the error test are no longer tracked.  New features, however,
% can be detected periodically.
%
% The tracked feature information is stored in a F x 4 x N matrix, where F
% is the total number of features detected throughout the video sequence
% and N is the number of frames in the image sequence.  For each feature at
% each frame, the 4-vector provides:
% y-position  - in pixels
% x-position  - in pixels
% error       - the error metric value for the feature in the current
% frame; this value ranges between 0 and 1, 1 representing the best
% tracking. This value is used to determine when tracking fails, so once
% the error metric for a particular feature drops below a certain
% threshold, the features is no longer considered valid and all subsequent
% error values should be 0.
% valid       - Indicates whether the feature is successfully tracked in
% the current frame
%
% Some progress information is provided to give some indication of how long
% the tracking process is likely to take.
%
% Input (default):
% imgs        - The image sequence on which to perform tracking
% sigmaS      - The spatial scale to use for image smoothing and image
% derivative computation (1.0)
% sigmaT      - The temporal scale to use for image smoothing and image
% derivative computation; NB: not currently used for anything; temporal
% derivatives are computed using frame differences. (1.0)
% count       - The maximum number of concurrent actively tracked features
% (500)
% featRadius  - The minimum distance in pixels between two features (5)
% featFrame   - The number of frames to wait before searching for new
% features to track (5)
% windRadius  - The radius of the feature window
% iters       - The number of linear equation iterations to use when
% tracking features
%
% Output:
% features    - The Fx4xN feature tracking matrix, as described above

display(sprintf('KLTTracker: %s \t Starting', datestr(now, 'HH:MM:SS')));

if (nargin < 8)
    iters = 1;
end;
if (nargin < 7)
    windRadius = 2;
end;
if (nargin < 6)
    featFrame = 5;
end;
if (nargin < 5)
    featRadius = 5;
end;
if (nargin < 4)
    count = 500;
end;
if (nargin < 3)
    sigmaT = 1.0;
end;
if (nargin < 2)
    sigmaS = 1.0;
end;

[h w t] = size(imgs);

% Find initial features
features(:,:,1) = HarrisDetector(imgs(:,:,1), sigmaS, sigmaS, count, featRadius);
displayFeatures(imgs(:,:,1), features(:,:,1));

for i=1:t-1
    % Find new features, if need be
    if (i > 1 && mod(i-1,featFrame) == 0)
        newFeat = HarrisDetector(imgs(:,:,i), sigmaS, sigmaS, count, featRadius, features(:,:,i));
        features(end:size(newFeat,1),:,:) = 0; % expand features
        features(:,:,i) = newFeat;
        clear newFeat;
    end;
    
    % Kill features that have gotten too close to the edge of the image
    fidx = find(features(:,4,end));
    features(fidx,4,end) = (features(fidx,1,end) > 1+featRadius).*(features(fidx,1,end) < h-featRadius).*(features(fidx,2,end) > 1+featRadius).*(features(fidx,2,end) < w-featRadius);

    % Track the features for one frame
    d = TrackFeatures(imgs(:,:,i), imgs(:,:,i+1), features(:,:,i), windRadius, sigmaS, iters);
    features(:,1:2,i+1) = features(:,1:2,i) + d;
    % Check the error on tracked features
    [err, val] = CheckError(imgs, features, 0.85, windRadius);
    features(:,3:4,i+1) = [err val];
    
    % Progress update
    display(sprintf('Progress %s: %3d of %3d (%3d%%), %5d of %5d features valid', datestr(now, 'HH:MM:SS'), i, t-1, round(100*i/(t-1)), sum(features(:,4,i+1)), size(features,1)));
end;
display(sprintf('KLTTracker: %s \t Finished', datestr(now, 'HH:MM:SS')));
