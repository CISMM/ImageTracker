function [ featImgs ] = ExtractFeatureImages( img, feats, featRadius )
% ExtractFeatureImages(img, feats, featRadius) - Creates a set of feature
% images from the input image and feature point locations.
% Inputs
%   img     - the input image from which to extract feature patches
%   feats   - the list of feature locations, a Fx4xT matrix, where F is the
%   feature index and T is a frame index
%   featRadius  - the radius [y,x] of the patches to extract

[h, w] = size(img);
fs = size(feats,1);

featImgs = zeros(2*featRadius(1)+1, 2*featRadius(2)+1, fs);

for fidx = 1:fs
    xx = max(feats(fidx,2,1),1)-featRadius(2) : 1 : min(feats(fidx,2,1),w)+featRadius(2);
    yy = max(feats(fidx,1,1),1)-featRadius(1) : 1 : min(feats(fidx,1,1),h)+featRadius(1);
    [xg, yg] = meshgrid(xx,yy);
    featImgs(:,:,fidx) = interp2(img,xg,yg);
end;
