function [ matches, scores ] = computeMatches( img1, feat1, img2, feat2, radius, minCorr )
%COMPUTEMATCHES Summary of this function goes here
%  Detailed explanation goes here

if (nargin < 5)
    radius = 20; % pixels to search in each direction
end;
if (nargin < 6)
    minCorr = 0.4; % minimum correlation to accept
end;

wWidth = 8;
rRatio = 5;

if (size(feat1, 2) < 8)
    warp = false;
    A1 = [1 0; 0 1];
    A2 = A1;
else
    warp = true;
end;

matchIds = []; scores = [];
jj = 1:size(feat2,1);

% Find pairs of points that are within the search radius
% We end up with a list of nearby feature indices.  The order
% of irFeat1 is in increasing feature point order.
for i=1:size(feat1,1)
    inRange(i,jj) = ((feat2(jj,1) >= feat1(i,1)-radius).*(feat2(jj,1) <= feat1(i,1)+radius).*(feat2(jj,2) >= feat1(i,2)-radius).*(feat2(jj,2) <= feat1(i,2)+radius))';
end;
[irFeat2, irFeat1] = find(inRange');
clear inRange;

% Check each pair of points for correspondence
lastPt = 0;
for index = 1:length(irFeat1)
    pt1 = irFeat1(index);
    pt2 = irFeat2(index);
    ptYX1 = feat1(pt1,1:2);
    ptYX2 = feat2(pt2,1:2);

    if (pt1 ~= lastPt) % Check if we've completed processing a point
        if (lastPt > 0 & bestMatch ~= 0)
            matchIds(end+1,:) = bestMatch;
            scores(end+1) = bestScore;
        end;
        bestScore = minCorr;
        bestMatch = 0;
    end;
    
    if (warp) % have to warp images
        if (lastPt ~= pt1) % haven't warped around pt 1 yet
            lastPt = pt1;
            A1 = reshape(feat1(pt1,3:6),2,2);
            t1 = feat1(pt1,8);
            R1 = [cos(t1) sin(t1); -sin(t1) cos(t1)];
            A1 = inv(R1) * A1;
            s1 = feat1(pt1,7);
            wWidth = ceil(s1 * rRatio);
            wind1 = warppatch2(img1,ptYX1',A1,wWidth);
        end;
        A2 = reshape(feat2(pt2,3:6),2,2);
        t2 = feat2(pt2,8);
        R2 = [cos(t2) sin(t2); -sin(t2) cos(t2)];
        A2 = inv(R2) * A2;    
    else % no warping
        if (lastPt ~= pt1)
            lastPt = pt1;
            wind1 = warppatch2(img1,ptYX1',A1,wWidth);
        end;
    end;
    wind2 = warppatch2(img2,ptYX2',A2,wWidth);
    
    score = NCCPatch(wind1, wind2);
    if (score > bestScore)
        bestScore = score;
        bestMatch = [pt1, pt2];
    end;        
end;

% Ensure that no feature point in image 2 is matched with more than one
% feature point in image 1.
[srt, ind] = sort(matchIds);

matches = [];
lastPt2 = 0;
for j = 1:length(srt)
    pt2 = srt(j,2);
    if (pt2 ~= lastPt2)
        if (lastPt2 ~= 0)
            pt1 = matchIds(bestInd,1);
            matches(end+1, :) = [feat1(pt1,1:2) feat2(lastPt2,1:2)];
        end;
        lastPt2 = pt2;
		bestInd = ind(j,2);
		bestScore = scores(bestInd);
    else
        if (scores(ind(j, 2)) > bestScore)
            bestInd = ind(j,2);
            bestScore = scores(bestInd);
        end;
    end;
end;

if (lastPt2 ~= 0)
    pt1 = matchIds(bestInd,1);
    matches(end+1, :) = [feat1(pt1,1:2) feat2(lastPt2,1:2)];
end;