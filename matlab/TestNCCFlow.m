%% Multilayer Cross Correlation
% Investigates the use of a normalized cross-correlation tracker for
% determining flow with mutliple motion layers.

%%
clear;
close all;
format compact;

%% Normalized cross correlation
% Cross correlation compares two images to find the shift that best brings
% those two images into alignment.  This metric can be used to track image
% patches from frame to frame.  We start with a simple video of rigid
% translation.  A Harris corner detector identifies a subset of features to
% track in the video.

% image frame parameters
outSize = [256 256];
middle = floor(outSize/2);
corner = [50 90];

% transform parameters
steps = 10;
scale = [1 1];
rotate = 0;
translate = [10 -10]*1;

% create an image sequence from a single image
img = double(rgb2gray(imread('davidbowie.jpg')));
imgs = TransformSequence(img, outSize, corner, corner+middle, scale, rotate, translate, steps);
[h, w, t] = size(imgs);

% add some noise
imgs = imgs + 10*randn(size(imgs));

% show the image sequence
figure(1);
dispimg(ImageMontage(imgs(:,:,[1:3:10]), 2, 5, 0));
% ShowMovie(imgs);

%%
% Set up tracking parameters.  An image feature consists of pixels within a
% small neighborhod, an image patch.  To find a feature within a frame we
% search for the best correspondance match between the feature an all
% possible image patches within a set radius of the previously known
% position of the feature.  If the best match within this region does not
% match the appearance of the feature, the feature is considered lost.
params.patchRadius = [8 8];
params.searchRadius = ceil(1.5*params.patchRadius);
params.featureThreshold = 0.90;
params.nccAccuracy = 1.0; 
params.errorTolerance = 0.65;
params.evolveFeatures = 0;
params.maxFeatures = 1000;
params.derivativeScale = 0.8;
params.integralScale = 2.0;

%%
% Initialize tracking features using the Harris feature detector, which
% finds regions of the image that have a large gradient in two directions
% (e.g. corners).
[featurePos, himg] = HarrisDetector(imgs(:,:,1), params, []);
features = CreateFeatures(featurePos);
features = ExtractFeatureImages(imgs(:,:,1), features, params.patchRadius);

patches = reshape([features.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
figure(1);
dispimg(ImageMontage(patches, 10, 1, 0));
title('Initial features to track');
figure(2);
dispimg(himg);
title('Interest feature image');

%% 
% Track using frame-to-frame feature matching with normalized cross
% correlation.
for frame = 2:t
    [features, corrMaps] = NCCFlow(imgs, [frame-1 frame], params, features);
    figure(2); 
    dispimg(ImageMontage(corrMaps, 10, 1, 0));
    title(sprintf('Correlation map frame %d', frame));
    drawnow;
end;

%%
% Evaluate tracking error.
active = reshape([features.active], [], length(features))';
fCount = size(active,1);
success = sum(active(:,end));
disp(sprintf('%d of %d (%3.2f %%) features tracked successfully', success, fCount, (100 * success / fCount)));

figure(2);clf;
error = reshape([features.error], [], length(features));
plot(error, 'b-');

%%
% Show tracked features over all frames.
figure(1); clf;
ShowFeatureMovie(imgs, features, 1);

%% Feature Evolution
% The previous example demonstrated how general feature detection and
% tracking using cross correlation can track image features.  With this
% approach, the feature appearance is determined when the feature is
% detected and is not changed throughout the tracking process.  Each
% frame, however, provides another view of each successfully tracked
% features.  We can therefore refine the appearance of an image patch with
% each new view; this reduces the contribution of noise in the patch
% appearance.  
%
% Feature refinement can improve the success rate of tracked
% features as well as improve the error metric for successfully-tracked
% features, potentially enabling tracking over longer ranges.  There is a
% danger, however, that tracking errors will accumulate and remain
% undetected as the appearance of a feature is adjusted to incorporate
% information from drifting off course.
params.evolveFeatures = 1;
[featurePos, himg] = HarrisDetector(imgs(:,:,1), params, []);
features = CreateFeatures(featurePos);
features = ExtractFeatureImages(imgs(:,:,1), features, params.patchRadius);
for frame = 2:t
    [features, corrMaps] = NCCFlow(imgs, [frame-1 frame], params, features);
    figure(2); 
    dispimg(ImageMontage(corrMaps, 10, 1, 0));
    title(sprintf('Correlation map frame %d', frame));
    drawnow;
end;

%%
% Display the original and the modified patches.
active = reshape([features.active], [], length(features))';
activeIdx = logical(active(:,end));
origPatches = reshape([features.originalPatch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
finalPatches = reshape([features.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
figure(1); clf;
subplot(1,2,1);
dispimg(ImageMontage(origPatches(:,:,activeIdx(1:100)), 10, 1, 0));
title('Initial tracked features');
subplot(1,2,2);
dispimg(ImageMontage(finalPatches(:,:,activeIdx(1:100)), 10, 1, 0));
title('Final tracked features');
drawnow;

%%
% Evaluate tracking error.
active = reshape([features.active], [], length(features))';
fCount = size(active,1);
success = sum(active(:,end));
disp(sprintf('%d of %d (%3.2f %%) features tracked successfully', success, fCount, (100 * success / fCount)));

figure(2);clf;
error = reshape([features.error], [], length(features));
plot(error, 'b-');

%%
% Show tracked features over all frames.
figure(1); clf;
ShowFeatureMovie(imgs, features, 1);

%%
% As a feature's appearance is modified, the error metric may improve as
% more noise is averaged out.  The accumuated feature patches can be used
% to refine tracking estimates from earlier in the image sequence as well.
% Here we show that re-running tracking does not improve the success rate,
% but does improve the error metric for successfully-tracked features.
params.evolveFeatures = 0; % we have already averaged all feature patches
for frame = 2:t
    [features, corrMaps] = NCCFlow(imgs, [frame-1 frame], params, features);
    figure(2); 
    dispimg(ImageMontage(corrMaps, 10, 1, 0));
    title(sprintf('Correlation map frame %d', frame));
    drawnow;
end;

%%
% Evaluate tracking error.
active = reshape([features.active], [], length(features))';
fCount = size(active,1);
success = sum(active(:,end));
disp(sprintf('%d of %d (%3.2f %%) features tracked successfully', success, fCount, (100 * success / fCount)));

figure(2);clf;
error = reshape([features.error], [], length(features));
plot(error, 'b-');

%% Multilayer Motion
% Cross correlation tracking can successfully track features from multiple
% layers in some image sequences.  We first construct a multilayer image
% sequence containing two translating images

% image frame parameters
outSize = [256 256];
middle = floor(outSize/2);
corner = [50 90];

% transform parameters
steps = 10;
scale = [1 1];
rotate = 0;
translate = [10 0] * 2.0;

% create an image sequence from a single image
img1 = double(rgb2gray(imread('davidbowie.jpg')));
img1 = ScaleData(img1, [0 255]);
imgs1 = TransformSequence(img1, outSize, corner, corner+middle, scale, rotate, translate, steps);

% create an image sequence from random splats
% corner = [250 100];
corner = floor(outSize/2);
translate = [0 10] * 2.0;
g = GaussianKernel1D(1.4, 0, 3);
gg = g'*g;
% img2 = RandomSplat(2*outSize, 0.97, gg);
img2 = double(rgb2gray(imread('einstein.jpg')));
img2 = ScaleData(img2, [0 255]);
imgs2 = TransformSequence(img2, outSize, corner, corner+middle, scale, rotate, translate, steps);

% create multilayer image
imgs = imgs1 + imgs2;
[h, w, t] = size(imgs);

% add some noise
imgs = imgs + 0*randn(size(imgs));

% show the image sequence
figure(1); clf;
dispimg(ImageMontage(imgs(:,:,round(linspace(1,t,4))), 2, 5, 0));
% ShowMovie(imgs);

%%
% Track using feature evolution.
params.evolveFeatures = 1;
[featurePos, himg] = HarrisDetector(imgs(:,:,1), params, []);
features = CreateFeatures(featurePos);
features = ExtractFeatureImages(imgs(:,:,1), features, params.patchRadius);
for frame = 2:t
    [features, corrMaps] = NCCFlow(imgs, [frame-1 frame], params, features);
    figure(2); 
    dispimg(ImageMontage(corrMaps, 10, 1, 0));
    title(sprintf('Correlation map frame %d', frame));
    drawnow;
end;

%%
% Display the original and the modified patches.  Note that the modified
% patches should provide a better view of a patch in one layer.
active = reshape([features.active], [], length(features))';
activeIdx = logical(active(:,end));
origPatches = reshape([features.originalPatch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
finalPatches = reshape([features.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
figure(1);
subplot(1,2,1);
dispimg(ImageMontage(origPatches(:,:,activeIdx), 10, 1, 0));
title('Initial tracked features');
subplot(1,2,2);
dispimg(ImageMontage(finalPatches(:,:,activeIdx), 10, 1, 0));
title('Final tracked features');
drawnow;

%%
% Display the modified image patches along with patches extracted from each
% of the original images.  Each tracked patch should match a patch in one
% of the original images.
featImg1 = ExtractFeatureImages(imgs1(:,:,1), features, params.patchRadius);
featImg2 = ExtractFeatureImages(imgs2(:,:,1), features, params.patchRadius);
img1Patches = reshape([featImg1.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
img2Patches = reshape([featImg2.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
figure(1);
subplot(1,3,1);
dispimg(ImageMontage(finalPatches(:,:,activeIdx), 7, 1, 0));
title('Tracked features');
subplot(1,3,2);
dispimg(ImageMontage(img1Patches(:,:,activeIdx), 7, 1, 0));
title('Layer 1 features');
subplot(1,3,3);
dispimg(ImageMontage(img2Patches(:,:,activeIdx), 7, 1, 0));
title('Layer 2 features');

%%
% Evaluate tracking error.
active = reshape([features.active], [], length(features))';
fCount = size(active,1);
success = sum(active(:,end));
disp(sprintf('%d of %d (%3.2f %%) features tracked successfully', success, fCount, (100 * success / fCount)));

figure(2);clf;
error = reshape([features.error], [], length(features));
plot(error, 'b-');

%%
% Show tracked features over all frames.
figure(1); clf;
ShowFeatureMovie(imgs, features, 1);

%% Opportunistic tracking
% Here we develop a more intelligent strategy for initializing tracking.
% Features are chosen from the second frame of the image sequence and
% tracked forwards and backwards.  Features are culled from this
% three-frame tracking, and feature appearance is established.  There is an
% attempt to find new features at each frame.
%
% This tracking method assumes that there are two different types of
% features detected by the Harris detector: features that are present
% because there is a strong feature in one of the image layers and features
% that are a side-effect of the combination of the image layers.  Featuers
% of the first type should be trackable while features of the second type
% should not be.

params.evolveFeatures = 1;
params.accuracy = 0.5;
features = NCCFlowMultiFrame(imgs, params);


%%
% Display the original and the modified patches.
origPatches = reshape([features.originalPatch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
finalPatches = reshape([features.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
figure(2); clf;
subplot(1,2,1);
dispimg(ImageMontage(origPatches, 20, 1, 0));
title('Initial tracked features');
subplot(1,2,2);
dispimg(ImageMontage(finalPatches, 20, 1, 0));
title('Final tracked features');
drawnow;

%%
% Evaluate tracking error.
active = reshape([features.active], [], length(features))';
fprintf('Features tracked for average of %f frames.\n', mean(sum(active, 2)));
error = reshape([features.error], [], length(features));
figure(3); clf;
plot(error, 'b-');

%%
% Show tracking movie.
figure(1); clf;
ShowFeatureMovie(imgs, features, 1);

%% Clustering
% Use RANSAC to determine which tracked points are in the same layer.
[h,w,t] = size(imgs);
fcount = size(features,2);
pos = reshape([features.pos], [], 2, fcount);
active = logical(reshape([features.active], [], fcount));

for f = 1:t-1
    f1 = f; f2 = f+1;
    activeFrame = find(active(f1,:) & active(f2,:));
    [tformA, ptsA] = RansacRigidTransform(squeeze(pos(f1,:,activeFrame)), squeeze(pos(f2,:,activeFrame)), 0.25, 0.95, 0.15);
    activeNotA = activeFrame(~ptsA);
    [tformB, ptsB] = RansacRigidTransform(squeeze(pos(f1,:,activeNotA)), squeeze(pos(f2,:,activeNotA)), 0.25, 0.95, 0.15);

    figure(1); clf;
    ShowFeatureMovie(imgs, features(activeFrame(ptsA)), 1);
    ShowFeatureMovie(imgs, features(activeNotA(ptsB)), 1);
end

%%
% Clustering possibility:
%   - consider entire set of features at once
%   - start with first frame pair and three points
%   - find transform and inliers
%       - threshold inlier count for continuing
%   - find set of features from inliers that are also in next frame
%   - find transfrom and inliers
%   - repeat for whole image sequence
%   - metric based on whole sequence matching
%       - total number of inliers 
%       - favor features that last long
%       - penalty for feature dropping out of layer or appearing in layer

% layerA = zeros(size(active,1)-1, size(active,2));
% for i = 1:size(active,1)-1
%     % find features active in two consecutive frames
%     activeFrame = find(active(i,:) & active(i+1,:));
% 
%     % find features in dominant layer
%     [tformA, ptsA] = RansacRigidTransform(squeeze(pos(i,:,activeFrame)),
%     squeeze(pos(i+1,:,activeFrame)), 0.25, 0.95, 0.20);
%     layerA(i,1:sum(ptsA)) = activeFrame(ptsA);
% end

