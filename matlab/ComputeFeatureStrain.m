function [ S, Lo, dL, L ] = ComputeFeatureStrain( feats, inds, frames )
%COMPUTESTRAIN Summary of this function goes here
%   Detailed explanation goes here

S = [0 0];
Lo = [0 0];
dL = [0 0];
L = [0 0];

% Ensure features exist in all frames
if (sum(sum(feats(inds,4,frames))) < 4)
    return;
end;

Lo = feats(inds(2), 1:2, frames(1)) - feats(inds(1), 1:2, frames(1));
L = feats(inds(2), 1:2, frames(2)) - feats(inds(1), 1:2, frames(2));
dL = L - Lo;
S = dL ./ L;
