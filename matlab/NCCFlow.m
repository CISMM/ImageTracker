function [ feat, meanFeat ] = NCCFlow( imgs, cFrame, tempRadius, searchRadius, feat, errTolerance, meanFeat )
% NCCFlow Computes the flow between two images using normalized
% cross-correlation block matching.

pFrame = cFrame - 1;
I1 = squeeze(imgs(:,:,pFrame));
I2 = squeeze(imgs(:,:,cFrame));

[h,w] = size(I1);

% feat should be an Fx4xT matrix;
fs = size(feat,1);

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
    
    if (ff(fidx) == 30 && cFrame == 6)
        disp('I am a jelly donut.');
    end;
    
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

    % find the maximum ncc response
    maxresp = max(corr(:));
    [i,j] = find(corr == maxresp);
    feat(ff(fidx), 1:2, cFrame) = [y+(j(1)-(searchRadius(1)+1)) x+(i(1)-(searchRadius(2)+1))];
    feat(ff(fidx), 3:4, cFrame) = [maxresp maxresp >= errTolerance];
    
%     if (feat(ff(fidx), 4)) % accepted feature
%         meanFeat(:,:,ff(fidx)) = (meanFeat(:,:,ff(fidx))*pFrame + imgTemp) / cFrame;
%     end;
end;