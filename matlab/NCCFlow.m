function [ feat, meanFeat ] = NCCFlow( imgs, cFrame, tempRadius, searchRadius, feat, errTolerance, meanFeat, evolve )
% NCCFlow Computes the flow between two images using normalized
% cross-correlation (NCC) block matching.
% Input (default)
%   imgs            - the images to track
%   cFrame          - the index of the current frame
%   tempRadius      - the radius of each feature template
%   searchRadius    - the radius of the image region in which to search for
%   each feature
%   feat            - feature list, a Nx4xF matrix, where N is the feature
%   index, F is the frame index, and the 4-vector specifies position (y,x),
%   error metric, and an active tracking flag (an even selection of points)
%   errTolerance    - the NCC error threshold below which features are
%   considered lost (0.95)
%   meanFeat        - a set of images of the feature templates [zeros]
%   evolve          - indicates whether the meanFeat images should be
%   updated with matched image regions from the current frame. 1 for
%   feature evolution, 0 for no evolution (0)
%
% Output
%   feat            - the updated feature matrix--will grow by one in the
%   frame direction
%   meanFeat        - the updated feature templates, updated only if
%   evolution is on
%

pFrame = cFrame - 1;
I1 = squeeze(imgs(:,:,pFrame));
I2 = squeeze(imgs(:,:,cFrame));

[h,w] = size(I1);

% feat should be an Fx4xT matrix;
fs = size(feat,1);

if (nargin < 8)
    evolve = 0;
end;
if (nargin < 7)
    meanFeat = zeros(2*tempRadius(1)+1, 2*tempRadius(2)+1, fs);
end;
if (nargin < 6)
    errTolerance = 0.95;
end;
if (nargin < 5) 
    % create a default feature list
    yy = searchRadius(1)+1:h-searchRadius(1);
    xx = searchRadius(2)+1:w-searchRadius(2);
    [xxg, yyg] = meshgrid(xx,yy);
    i = 1:100:length(yy)*length(xx);
    feat = [yyg(i); xxg(i); ones(length(i)); ones(length(i))]';
end;

ff = find(feat(:,4,pFrame));

for fidx = 1:length(ff)
    y = feat(ff(fidx),1,pFrame);
    x = feat(ff(fidx),2,pFrame);
    
    % compute the indices of the search window and template
    yySearch = max(y-searchRadius(1),1):min(y+searchRadius(1), h);
    xxSearch = max(x-searchRadius(2),1):min(x+searchRadius(2), w);
%     yyTemp = max(y-tempRadius(1),1):min(y+tempRadius(1), h);
%     xxTemp = max(x-tempRadius(2),1):min(x+tempRadius(2), w);

%     if (size(yyTemp,2) ~= 2*tempRadius(1)+1 || size(xxTemp,2) ~= 2*tempRadius(2)+1)
%         continue;
%     end;
    
    % take the template from the mean template, search for it in I2
    imgSearch = I2(yySearch, xxSearch);
%     imgTemp = I1(yyTemp, xxTemp);
    imgTemp = squeeze(meanFeat(:,:,ff(fidx)));
    corr = CorrMap(imgSearch, imgTemp);

    if (y - searchRadius(1) < 1)
        dp(1) = 2*searchRadius(1) + 1 - size(yySearch,2);
    else
        dp(1) = 0;
    end;
    if (x - searchRadius(2) < 1)
        dp(2) = 2*searchRadius(2) + 1 - size(xxSearch,2);
    else
        dp(2) = 0;
    end;
    
    % find the maximum ncc response
    maxresp = max(corr(:));
    [j,i] = find(corr == maxresp); % find returns row, col
    feat(ff(fidx), 1:2, cFrame) = [y+(j(1)-(searchRadius(1)+1)) x+(i(1)-(searchRadius(2)+1))] + dp;
    feat(ff(fidx), 3:4, cFrame) = [maxresp maxresp >= errTolerance];
    
    % Evolve the tracked feature
    if (evolve && feat(ff(fidx), 4)) % accepted feature
        match = imgSearch(j-tempRadius(1):j+tempRadius(1),i-tempRadius(2):i+tempRadius(2));
        meanFeat(:,:,ff(fidx)) = (meanFeat(:,:,ff(fidx))*pFrame + match) / cFrame;
    end;
end;