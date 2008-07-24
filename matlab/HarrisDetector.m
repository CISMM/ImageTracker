function [ featurePos, himg ] = HarrisDetector( img, params, featurePos )
% [ features, himg ] = HarrisDetector( img, scaleD, scaleI, count ) - Harris
% interest feature point detector.
%
% Finds Harris corner features in a given image. If featurePos is supplied
% only new features not near existing features are added to the feature
% list.
%
% Input (default):
%   img         - The input image
%   params      - detector parameters including:
%       .derivativeScale	The derivative scale at which to detect
%       features
%       .integralScale      The integration scale over which to sum
%       features
%       .maxFeatures        The targeted number of feature points
%       .searchRadius       The radius around an established feature where
%       no other features should be selected (5)
%       .featureThreshold   The percentage of maximum below which to ignore
%       features (0.05)
%   featurePos  - existing features, an Nx2 array
%
% Output:
%   features    - the location of features in an Nx2 array
%   himg        - the computed Harris feature image

% Default parameters
if (nargin < 3)
    featurePos = [];
end
if (nargin < 2)
    params = [];
end

featureRadius = GetFieldDefault(params, 'patchRadius', 5);
featureRadius = max(featureRadius);

% Create some Gaussian filters
G = GaussianKernel1D(GetFieldDefault(params, 'derivativeScale', 1.0), 0, 3);
dG = GaussianKernel1D(GetFieldDefault(params, 'derivativeScale', 1.0), 1, 3);

% Compute the x and y derivatives of the input image
Ix = filter2(G'*dG, img);
Iy = filter2(dG'*G, img);

% figure; dispimg(Ix, 'Ix');
% figure; dispimg(Iy, 'Iy');

% Compute the components of the structure tensor
Ixx = Ix.*Ix;
Iyy = Iy.*Iy;
Ixy = Ix.*Iy;

% figure; dispimg(Ixx, 'Ixx');
% figure; dispimg(Iyy, 'Iyy');
% figure; dispimg(Ixy, 'Ixy');

% Integrate the components of the structure tensor over the integration
% window, weighted with a Gaussian function.
Gi = GaussianKernel1D(GetFieldDefault(params, 'integralScale', 1.0), 0, 3);
GG = Gi'*Gi;
Gxx = filter2(GG, Ixx);
Gyy = filter2(GG, Iyy);
Gxy = filter2(GG, Ixy);

% figure; dispimg(Gxx, 'Gxx');
% figure; dispimg(Gyy, 'Gyy');
% figure; dispimg(Gxy, 'Gxy');

% Harris feature image function
himg = (Gxx.*Gyy - Gxy.^2) - 0.06*(Gxx + Gyy).^2;
hcutoff = max(himg(:)) - range(himg(:)) * GetFieldDefault(params, 'featureCutoff', 0.90);

% figure; dispimg(himg, 'Harris');

% Find local maxima in the Harris feature image
[h, w] = size(img);
M = zeros(h,w);
ii = 1 + featureRadius:h - featureRadius;
jj = 1 + featureRadius:w - featureRadius;
% M marks the possible location of Harris features--local maxima
M(ii,jj) = (himg(ii,jj) > hcutoff) .* ...
    (himg(ii, jj) > himg(ii-1, jj-1)) .* ...
    (himg(ii, jj) > himg(ii-1, jj)) .* ...
    (himg(ii, jj) > himg(ii-1, jj+1)) .* ...
    (himg(ii, jj) > himg(ii,   jj-1)) .* ...
    (himg(ii, jj) > himg(ii,   jj+1)) .* ...
    (himg(ii, jj) > himg(ii+1, jj-1)) .* ...
    (himg(ii, jj) > himg(ii+1, jj)) .* ...
    (himg(ii, jj) > himg(ii+1, jj+1));

% Handle existing features
if (~isempty(featurePos))  
    % Zero out regions in the feature map that are close to existing
    % features
    for idx=1:size(featurePos,1)
        ii = max(1, featurePos(idx,1) - featureRadius): ...
            min(h,featurePos(idx,1) + featureRadius);
        jj = max(1, featurePos(idx,2) - featureRadius): ...
            min(w,featurePos(idx,2) + featureRadius);
        M(round(ii),round(jj)) = 0;
    end;
end;

% hScore preserves the Harris feature scores only at local maxima
hScore = himg.*M;

% Find indices of non-zero scores, and sort according to score
[si, sj, sVal] = find(hScore);
[tmp, sortIdx] = sort(sVal,1,'descend');

% Add new features if they are not too close to existing features
idx = 1;
while ((isempty(featurePos) || size(featurePos,1) < GetFieldDefault(params, 'maxFeatures', 1000)) && idx <= length(sortIdx))
    % Get the feature's position
    pos = [si(sortIdx(idx)) sj(sortIdx(idx))];
    % Check if feature is still valid
    if (M(pos(1),pos(2)) > 0)
        featurePos(end+1, :) = pos;
        % clear the neigborhood around features
        ii = max(1, featurePos(end,1) - featureRadius): ...
            min(h, featurePos(end,1) + featureRadius);
        jj = max(1, featurePos(end,2) - featureRadius): ...
            min(w, featurePos(end,2) + featureRadius);
        M(ii,jj) = 0;
    end;
    idx = idx+1;
end;
