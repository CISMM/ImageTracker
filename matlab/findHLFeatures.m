function [ features ] = findHLFeatures( img, levels, maxCount, scaleInit, scaleFact )
%HLFEATURES Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 5)
    scaleFact = 1.2;
end;
if (nargin < 4)
    scaleInit = 2.5;
end;
if (nargin < 3)
    maxCount = 1000;
end;
if (nargin < 2)
    levels = 8;
end;

Lthresh = -10;
[height, width] = size(img);

Lspace(1,:,:) = -inf * ones(height,width);
for level = 1:levels
    scaleI = scaleInit * scaleFact ^ (level-1);
    scaleD = 0.7 * scaleI;
    
    % Find Harris points at this scale
    hcurrent = harrisAtScale(img, scaleI, scaleD, maxCount);
    diff = maxCount - length(hcurrent);
    if (diff > 0)
        hcurrent(end+1:maxCount, 1:2) = zeros(diff, 2);
    end;
    hpoints(level,:,:) = hcurrent;
    
    % Compute the Laplacian at this scale
    LoG = fspecial('log', ceil(scaleI * 8), scaleI);
    Lspace(end+1,:,:) = conv2(img, LoG, 'same');
end;
Lspace(end+1,:,:) = -inf * ones(height, width);

features = [];
% Combine Harris points with Laplace scale info.  For each Harris
% coordinate point, we want to check if the Laplacian also obtains a local
% maximum at the corresponding scale.  If it does not, we discard the
% point.
for lev = 1:size(hpoints,1)
    scale = scaleInit * scaleFact ^ (lev-1);
    points = squeeze(hpoints(lev,:,:));
    for id = 1:size(points,1);
        x = points(id,2);
        y = points(id,1);
        if (max([x,y]) > 0 && Lspace(lev+1,y,x) >= Lthresh && Lspace(lev+1,y,x) == max(Lspace(lev:lev+2,y,x)))
            features(end+1,:) = [y, x, scale];
        end;
    end;
end;