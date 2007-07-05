function [ ] = ShowFeatureFrame( imgs, feats, index )
% ShowFeatureFrame(imgs, feats, index) Displays a single frame of an image
% sequence along with any features tracked in that frame.

idx = max(1,index);
idx = min(idx, size(imgs,3));

[hi,wi,ti] = size(imgs);
[hf,wf,tf] = size(feats);
ff = 1:hf;

% show image
dispimg(imgs(:,:,idx));

hold on;
if (idx == 1)    
    % Show first frame
    fidx = find(feats(:,4,idx));
    plot(feats(fidx,2,1), feats(fidx,1,1), 'gs');
else
    % Show other frames
    % Determine feature classes
    newFeat(ff)  = (feats(ff,4,idx) == 1).*(feats(ff,4,idx-1)==0);
    contFeat(ff) = (feats(ff,4,idx) == 1).*(feats(ff,4,idx-1)==1);
    deadFeat(ff) = (feats(ff,4,idx) == 0).*(feats(ff,4,idx-1)==1);
    newIdx  = find(newFeat);
    contIdx = find(contFeat);
    deadIdx = find(deadFeat);
    
    % display the features, color coded
    plot(feats(newIdx, 2,idx), feats( newIdx,1,idx), 'gs');
    plot(feats(contIdx,2,idx), feats(contIdx,1,idx), 'ys');
    plot(feats(deadIdx,2,idx), feats(deadIdx,1,idx), 'rs');
end;

hold off; % done drawing on the figure
