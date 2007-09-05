function [ err, val ] = CheckError( imgs, feat, tolerance, radius, prev )
% CheckError(imgs, feat, tolerance, radius) Computes the error in tracking
% a feature.
%
% Given a set of features in I1 and a set of corresponding displacements of
% each feature to I2, this function computes the error (normalized c,ross
% correlation) in each image patch.  Additionally, a validity flag is set
% for the feature if the computed error is above the specified tolerance.
%
% Input (default):
% imgs        - The image sequence
% feat        - The set of features
% tolerance   - The error tolerance threshold (0.85)
% radius      - The image patch radius (2)
% prev        - Indicates that a feature should be compared to the previous
% frame instead of the first frame in which the feature appeared (0)
%
% Output
% err         - The set of errors corresponding to each feature patch
% val         - A flag indicating whether each feature patch was within the
% error tolerance

if (nargin < 5)
    prev = 0;
end;
if (nargin < 4)
    radius = 2;
end;
if (nargin < 3)
    tolerance = 0.85;
end;

err = zeros(size(feat,1),1);
val = zeros(size(feat,1),1);

% The current image frame
cframe = size(feat,3);

% Find the valid feature indices
fidx = find(feat(:,4,end-1));

for i=1:length(fidx)
    idx = fidx(i); % the current valid feature index
    
    % Find the feature's first frame
    if (prev)
        fframe = cframe - 1;
    else
        fframe = find(feat(idx,4,:), 1);
    end;
    
    % Create an image patch by sampling I2 using the computed
    % displacement for each image feature.
    yy1 = feat(idx,1,fframe)-radius:1:feat(idx,1,fframe)+radius;
    xx1 = feat(idx,2,fframe)-radius:1:feat(idx,2,fframe)+radius;
    yy2 = feat(idx,1,cframe)-radius:1:feat(idx,1,cframe)+radius;
    xx2 = feat(idx,2,cframe)-radius:1:feat(idx,2,cframe)+radius;
    [xidx1, yidx1] = meshgrid(xx1,yy1);
    [xidx2, yidx2] = meshgrid(xx2,yy2);

    % Compute the image patches
    pI1 = interp2(imgs(:,:,fframe),xidx1,yidx1);
    pI2 = interp2(imgs(:,:,cframe),xidx2,yidx2);

    % Compute the error
    err(idx) = NCCpatch(pI1, pI2, 0);
%         err(i) = sum(sum((pI2-pI1).^2));
    val(idx) = err(idx) > tolerance;
end;
