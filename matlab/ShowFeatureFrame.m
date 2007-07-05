function [ ] = ShowFeatureFrame( imgs, feats, index )
% ShowFeatureFrame(imgs, feats, index) Displays a single frame of an image
% sequence along with any features tracked in that frame.

idx = max(1,index);
idx = min(idx, size(imgs,3));
dispimg(imgs(:,:,idx));

fidx = find(feats(:,4,idx));
hold on;
plot(feats(fidx,2,idx), feats(fidx,1,idx), 'ys');
hold off;
