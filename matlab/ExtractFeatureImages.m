function [ feats ] = ExtractFeatureImages( img, feats, featRadius, frame )
% ExtractFeatureImages(img, feats, featRadius) - Creates a set of feature
% images from the input image and feature point locations.
% Inputs
%   img     - the input image from which to extract feature patches
%   feats   - the structure array features
%   featRadius  - the radius [y,x] of the patches to extract

if (nargin < 4)
    frame = 1;
end

[h, w] = size(img);
fs = length(feats);

for fidx = 1:fs
    xx = max(feats(fidx).pos(frame,2), 1) - featRadius(2) : ...
        min(feats(fidx).pos(frame,2), w) + featRadius(2);
    yy = max(feats(fidx).pos(frame,1), 1) - featRadius(1) : ...
        min(feats(fidx).pos(frame,1), h) + featRadius(1);
    [xg, yg] = meshgrid(xx,yy);
    feats(fidx).originalPatch = interp2(img, xg, yg);
    feats(fidx).patch = feats(fidx).originalPatch;
end;
