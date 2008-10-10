function [flow] = ComputeFlow(imgs)
%% ComputeFlow
% Computes the optical flow for a set of input images.

[h, w, t] = size(imgs);

% initialize flow fields
flow = zeros(h, w, 2, t-1);

% compute flow for each frame
for frame = 1:t-1
    fprintf('Frame: %d of %d (%6.1f)', frame, t-1, (100 * frame/(t-1)));
    [flow(:,:,1,frame), flow(:,:,2,frame)] = MultiResOpticalFlow(imgs(:,:,frame), imgs(:,:,frame+1), 1.0, 4.0, 80, 50, 4);
    ShowVectorField(flow(:,:,:,frame), 485, 10, imgs(:,:,frame));
    pause(0.5);
end;

