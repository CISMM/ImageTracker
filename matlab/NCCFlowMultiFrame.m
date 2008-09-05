function [features] = NCCFlowMultiFrame(imgs, params)
% NCCFlowMultiFrame tracks features that can be identified over multiple
% frames of an image sequence.  Features are chosen from the second frame
% of the image sequence and tracked forwards and backwards.  Features are
% culled from this three-frame tracking, and feature appearance is
% established.  There is an attempt to find new features at each frame.

[h, w, t] = size(imgs);

features = [];
curFeaturePos = [];

for frame = 2:t-1
    % find new feature positions
    featurePos = HarrisDetector(imgs(:, :, frame), params, curFeaturePos);
    newFeatures = CreateFeatures(featurePos(size(curFeaturePos,1)+1:end,:), frame);
    newFeatures = ExtractFeatureImages(imgs(:, :, frame), newFeatures, params.patchRadius, frame);

    % search backward and forward
    newFeatures = NCCFlow(imgs, [frame frame-1], params, newFeatures);
    newFeatures = NCCFlow(imgs, [frame frame+1], params, newFeatures);

    % cull features that don't survive for three frames
    active = reshape([newFeatures.active], [], length(newFeatures))';
    activeIdx = sum(active,2) == 3;
    newFeatures(~activeIdx) = [];

    % display new features
    if (~isempty(newFeatures) > 0)
        patches = reshape([newFeatures.patch], 2*params.patchRadius(1)+1, 2*params.patchRadius(2)+1, []);
        figure(1); clf;
        dispimg(ImageMontage(patches, 10, 1, 0));
        title(sprintf('New features frame %i', frame)); drawnow;
    end
    
    % track existing features for this frame
    if (~isempty(features))
        [features, corrMaps] = NCCFlow(imgs, [frame, frame+1], params, features);
        figure(2); clf;
        dispimg(ImageMontage(corrMaps, 15, 1, 0));
        title(sprintf('Correlation map frame %d', frame));
        drawnow;

        % add new features to set of tracking features
        newCount = length(newFeatures);
        features(end+1:end+newCount) = newFeatures;
    else
        features = newFeatures;
    end

    % keep track of positions of active features
    active = reshape([features.active], [], length(features))';
    activeIdx = logical(active(:,end));
    pos = reshape([features.pos], [], 2, length(features));
    curFeaturePos = squeeze(pos(end, :, activeIdx))';
end
