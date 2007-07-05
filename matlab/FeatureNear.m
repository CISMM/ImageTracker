function [ feat ] = FeatureNear( feats, pt )
% FeatureNear(feats, pt) Finds the feature index closest to the given
% location.

fidx = find(feats(:,4));
d = sqrt((feats(fidx,1)-pt(1)).^2 + (feats(fidx,2)-pt(2)).^2);
[mini, i] = min(d);
feat = fidx(i);