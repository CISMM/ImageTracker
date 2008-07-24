function [ features, corrMaps ] = NCCFlow(imgs, frameIdx, params, features )
% NCCFlow Computes the flow between two images using normalized
% cross-correlation (NCC) block matching.
% Input (default)
%   imgs            - the images to track
%   frameIdx        - the indeces of the frames to track
%   params          - tracking parameters, including:
%       .patchRadius    - the radius of each feature patch
%       .searchRadius   - the radius of the matching search region
%       .errorTolerance - the NCC error threshold below which features are
%       considered lost
%       .evolveFeatures - indicates whether the feature patches should be
%       updated with matched image regions
%       .accuracy       - the subpixel accuracy to use
%   features        - the structure array of features, with fields
%       .pos        - the Nx2 positions of a feature, where N is the frame
%       index and the positions are stored (y,x) in each row
%       .error      - the Nx1 error metric values for the feature
%       .active     - a Nx1 array indicating if a feature is valid in a
%       given frame
%       .patch      - the patch appearance
%
% Output
%   features        - the updated feature structure array
%   corrMaps        - a p x p x F matrix of correlation maps, where p is
%   the width of an image patch and F is the active feature index

pFrame = frameIdx(1);
cFrame = frameIdx(2);

I1 = squeeze(imgs(:, :, pFrame));
I2 = squeeze(imgs(:, :, cFrame));

[h,w] = size(I1);

fcount = length(features);
corrMaps= zeros(2*params.searchRadius(1)+1, 2*params.searchRadius(2)+1, fcount);

accuracy = GetFieldDefault(params, 'nccAccuracy', 1);

for fidx = 1:fcount
    if (features(fidx).active(pFrame)) % process only active features
        y = features(fidx).pos(pFrame,1);
        x = features(fidx).pos(pFrame,2);
        
        % exclude features that have search windows that run off the edge
        % of the frame
        if (y - params.searchRadius(1) < 1 || ...
            y + params.searchRadius(1) > h || ...
            x - params.searchRadius(2) < 1 || ...
            x + params.searchRadius(2) > w)
                features(fidx).pos(cFrame,:) = [0 0];
                features(fidx).error(cFrame) = 0;
                features(fidx).active(cFrame) = 0;
                continue;
        end

        % compute the indices of the search window and template
        yySearch = ...
            y-params.searchRadius(1): ...
            y+params.searchRadius(1);
        xxSearch = ...
            x-params.searchRadius(2): ...
            x+params.searchRadius(2);
        [xxMesh, yyMesh] = meshgrid(xxSearch, yySearch);

        % take the template from the mean template, search for it in I2
        imgSearch = interp2(I2, xxMesh, yyMesh);
        imgPatch = features(fidx).patch;
        corr = CorrMap(imgSearch, imgPatch);

        corrMaps(1:size(corr,1), 1:size(corr,2), fidx) = corr;

        % find the maximum ncc response
        maxresp = max(corr(:));
        [j,i] = find(corr == maxresp); % find returns row, col
        
        % Subpixel correlation
        % resample the template and search image at the desired subpixel
        % accuracy
        if (accuracy < 1 & accuracy > 0)
            % resample search image
            x = x + (i(1) - (params.searchRadius(2) + 1));
            y = y + (j(1) - (params.searchRadius(1) + 1));
            yySearch = ...
                y - params.patchRadius(2) - 1:accuracy: ...
                y + params.patchRadius(2) + 1;
            xxSearch = ...
                x - params.patchRadius(1) - 1:accuracy: ...
                x + params.patchRadius(1) + 1;
            [xxMesh, yyMesh] = meshgrid(xxSearch, yySearch);
            imgSearch = interp2(I2, xxMesh, yyMesh);
            
            % resample template image
            yyPatch = 1:accuracy:size(imgPatch,1);
            xxPatch = 1:accuracy:size(imgPatch,2);
            [xxMesh, yyMesh] = meshgrid(xxPatch, yyPatch);
            imgPatch = interp2(imgPatch, xxMesh, yyMesh);
            
            % compute correlation map and find maximum ncc response
            corr = CorrMap(imgSearch, imgPatch);
            maxresp = max(corr(:));
            [j,i] = find(corr == maxresp); % find returns row, col
            
            features(fidx).pos(cFrame,:) = ...
                [y + (j(1) - (size(corr,1)+1)/2) * accuracy
                 x + (i(1) - (size(corr,2)+1)/2) * accuracy];
        else
            features(fidx).pos(cFrame,:) = ...
                [y + (j(1) - (params.searchRadius(1) + 1)) ...
                 x + (i(1) - (params.searchRadius(2) + 1))];
        end
        features(fidx).error(cFrame) = maxresp;
        features(fidx).active(cFrame) = maxresp > params.errorTolerance;

        % Evolve the tracked feature
        if (params.evolveFeatures && features(fidx).active(cFrame)) % accepted feature
            activeCount = sum([features(fidx).active]);
            match = imgSearch(j-params.patchRadius(1):j+params.patchRadius(1),i-params.patchRadius(2):i+params.patchRadius(2));
            features(fidx).patch = (features(fidx).patch * (activeCount-1) + match) ./ activeCount;
        end
    else
        features(fidx).pos(cFrame,:) = [0 0];
        features(fidx).error(cFrame) = 0;
        features(fidx).active(cFrame) = 0;
    end
end
