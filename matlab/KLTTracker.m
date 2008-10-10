function [ features ] = KLTTracker( imgs, params )
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
% The tracked feature information is stored in an F-element feature
% structure array, where F is the total number of features detected
% throughout the video sequence. For each feature the structure provides:
%
%   features        - the structure array of features, with fields
%       .pos        - the Nx2 positions of a feature, where N is the frame
%       index and the positions are stored (y,x) in each row
%       .error      - the Nx1 error metric values for the feature
%       .active     - a Nx1 array indicating if a feature is valid in a
%       given frame this value ranges between 0 and 1, 1 representing the
%       best tracking. This value is used to determine when tracking fails,
%       so once the error metric for a particular feature drops below a
%       certain threshold, the features is no longer considered valid and
%       all subsequent error values should be 0
%       .patch      - the patch appearance when first detected
%
% Some progress information is provided to give some indication of how long
% the tracking process is likely to take.
%
% Input (default):
% imgs -                The image sequence on which to perform tracking
% params -              A parameters structure with the following elements:
%   .derivativeScale -  the spatial scale to use for derivative computation
%   .integralScale -    the spatial scale to use for image smoothing
%   .featureRadius -    the minimum distance in pixels between two features
%   .featureFrames -    the number of frames to wait before searching for
%   new features to track
%   .maxFeatures -      the maximum number of actively tracked features
%   .featureThreshold - the Harris feature detector value below which to
%   reject features
%   .windowRadius -     the radius of the window to use when computing
%   patch displacements
%   .iterations -       the number of iterations to use when solving for
%   motion
%   .errorTolerance -   the error threshold for accepting a tracked feature
%   .updateFeature -    whether to update the feature appearances at each
%   frame instead of always comparing to the first feature appearance.
%
% Output
% features -    the feature structure array as described above

display(sprintf('KLTTracker: %s \t Starting', datestr(now, 'HH:MM:SS')));

if (nargin < 2)
    params = [];
end

% setup default parameters
[v, params] = GetFieldDefault(params, 'derivativeScale', 1.0);
[v, params] = GetFieldDefault(params, 'integralScale', 2.4);
[v, params] = GetFieldDefault(params, 'featureRadius', [2 2]);
[v, params] = GetFieldDefault(params, 'searchRadius', ceil(2.5 * params.featureRadius));
[v, params] = GetFieldDefault(params, 'featureFrames', 5);
[v, params] = GetFieldDefault(params, 'maxFeatures', 500);
[v, params] = GetFieldDefault(params, 'featureCutoff', 0.80);
[v, params] = GetFieldDefault(params, 'iterations', 2);
[v, params] = GetFieldDefault(params, 'errorTolerance', 0.65);
[v, params] = GetFieldDefault(params, 'updateFeature', 0);
disp(params);

[h w t] = size(imgs);

% Find initial features
featurePos = HarrisDetector(imgs(:,:,1), params);
features = CreateFeatures(featurePos);
features = ExtractFeatureImages(imgs(:,:,1), features, params.featureRadius);

fprintf('Harris features: %d\tCreatedFeatures %d\n', length(featurePos), length(features));

[ph, pw] = size(features(1).patch);
patches = reshape([features.patch], ph, pw, []);

figure(1); clf;
subplot(2,1,1)
displayFeatures(imgs(:,:,1), featurePos);
subplot(2,1,2);
dispimg(ImageMontage(patches, 20, 1, 0));
drawnow;

for i=1:t-1
    % Find new features, if need be
    if (i > 1 && mod(i-1,params.featureFrames) == 0)
        N = length(features);
        position = reshape([features.pos], [], 2, N);   % i x 2 x N matrix
        active = logical(reshape([features.active], [], N));     % i x N matrix
        oldPos = squeeze(position(end, :, active(end,:)))';
        newPos = HarrisDetector(imgs(:,:,i), params, oldPos);
        oldCnt = length(oldPos);
        newCnt = length(newPos) - oldCnt;
        newIdx = N+1:N+newCnt;
        features = CreateFeatures(newPos(oldCnt+1:end,:), i, features);
        features(newIdx) = ExtractFeatureImages(imgs(:,:,i), features(newIdx), params.featureRadius, i);
    end;
    
    % Track the features for one frame
    features = KLTTrackerStep(imgs, [i i+1], features, params);
    
    % Progress update
    N = length(features);
    active = reshape([features.active], [], N);
    nowActive = sum(active(end,:));
    display(sprintf('Progress %s: %3d of %3d (%3d%%), %5d of %5d features valid', datestr(now, 'HH:MM:SS'), i, t-1, round(100*i/(t-1)), nowActive, N));
end;
display(sprintf('KLTTracker: %s \t Finished', datestr(now, 'HH:MM:SS')));
