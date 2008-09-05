function features = CreateFeatures(featurePos, frame, features)
if (nargin < 3)
    features = [];
end
if (nargin < 2)
    frame = 1;
end

for i = 1:size(featurePos,1)
    features(end+1).pos(frame,:) = featurePos(i,:);
    features(end).error(frame) = 1;
    features(end).active(frame) = 1;
end