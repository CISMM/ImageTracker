function [bestTform, bestIds] = RansacRigidTransform(pointsA, pointsB, errorTol, assurance, probPoint)
% RansacRigidTransform finds a rigid transform that best explains a
% transfrom between 2D pointsA and pointsB.
%
% Input:
%   pointsA     - the set of N fixed points A, 2 x N
%   pointsB     - the set of moving points B, 2 x N
%   errorTol    - the error threshold for a single point
%   assurance   - the desired percent assurance that the resultant
%   transfrom is correct (0 - 1)
%   probPoint   - the probability that any random point pair is in the
%   transform (0 - 1)
% Output:
%   bestTform   - the best transfrom
%   bestIds     - a logical vector of points that are within this transfrom

N = size(pointsA, 2);
if N ~= size(pointsB, 2)
    warning('Number of points must be equal.%d ~= %d', N, size(pointsB,2));
    return;
end

% define a vectorized distance function for homogonized coordinates
distance = @(a, b) sqrt(sum((a - b).^2, 1));

% homogenize points
homogA = [pointsA; ones(1, N)];
homogB = [pointsB; ones(1, N)];

% determine the maximum number of tries to make
maxTries = ceil(log(1 - assurance)/log(1 - probPoint^3));

% intialize best soluton seen so far
bestTform = eye(3);
bestIds = [];
bestCount = 0;
tries = 0;

while tries < maxTries
    tries = tries + 1;
    % randomly pick a set of points indices
    ids = [0 0 0];
    while (length(unique(ids)) ~= 3)
        ids = ceil(N * rand(1,3));
    end
    
    % determine a transform for these three points pairs A = TB
    % Linearize as Mx = b, where x provides transform T.
    M = ...
        [homogB(:,ids)' zeros(3,3)
         zeros(3,3) homogB(:,ids)'];
    b = [homogA(1,ids)'
         homogA(2,ids)'];

    % need at least rank 6 matrix
    if (rank(M) < 6)
        continue;
    end
    
    % solve for transfrom parameters and arrange as a 2D transform matrix
    x = M\b;
    thisTform = [reshape(x, 3, 2)'; 0 0 1];
    
    % find other points that agree with this transform model
    thisDist = distance(homogA, thisTform*homogB);
    thisIds = thisDist <= errorTol;
    lastIds = []; refCount = 0;
    
    % refine model until stable
    while (~isequal(thisIds, lastIds) & refCount < 30)
        lastIds = thisIds;
        refCount = refCount + 1;
        
        % linear regression
        M = ...
            [homogB(:,thisIds)' zeros(sum(thisIds), 3)
             zeros(sum(thisIds), 3) homogB(:,thisIds)'];
        b = [homogA(1, thisIds)'
             homogA(2, thisIds)'];
        x = M\b;
        thisTform = [reshape(x, 3, 2)'; 0 0 1];
        
        % find new points agree with transform
        thisDist = distance(homogA, thisTform*homogB);
        thisIds = thisDist <= errorTol;
    end
    
    % update our solution if this is the best seen so far
    if (sum(thisIds) > bestCount)
        bestIds = thisIds;
        bestCount = sum(thisIds);
        bestTform = thisTform;
        
        % update our number of attempts based on how many inliers there are
        probPoint = bestCount / N;
        maxTries = ceil(log(1 - assurance)/log(1 - probPoint^3));
    end
end

fprintf('Best transform found in %d tries with %0.3f %% inliers\n', tries, (100 * bestCount/N));
