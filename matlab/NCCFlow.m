function [ nccFeat ] = NCCFlow( I1, I2, tempRadius, searchRadius, feat )
% NCCFlow Computes the flow between two images using normalized
% cross-correlation block matching.

[h,w] = size(I1);

if (nargin < 5) 
    % create a default feature list
    yy = searchRadius(1)+1:h-searchRadius(1);
    xx = searchRadius(2)+1:w-searchRadius(1);
    [xxg, yyg] = meshgrid(xx,yy);
    i = 1:100:length(yy)*length(xx);
    feat = [yyg(i); xxg(i); zeros(length(i))]';
end;

% feat should be an Nx2 matrix;
fs = size(feat,1);
nccFeat = zeros(fs,4);

for fidx = 1:fs
    y = feat(fidx,1);
    x = feat(fidx,2);
    
    % compute the indices of the search window and template
    yySearch = max(y-searchRadius(1),1):min(y+searchRadius(1), h);
    xxSearch = max(x-searchRadius(2),1):min(x+searchRadius(2), w);
    yyTemp = max(y-tempRadius(1),1):min(y+tempRadius(1), h);
    xxTemp = max(x-tempRadius(2),1):min(x+tempRadius(2), w);

    % take the template from I1, search for it in I2
    imgSearch = I2(yySearch, xxSearch);
    imgTemp = I1(yyTemp, xxTemp);
    corr = CorrMap(imgSearch, imgTemp);

    % find the maximum ncc response
    [i,j] = find(corr == max(corr(:)));
    nccFeat(fidx, 1:2) = [y+(j(1)-(searchRadius(1)+1)) x+(i(1)-(searchRadius(2)+1))];
    nccFeat(fidx, 3:4) = [max(corr(:)) 1];
end;