function [ features, himg ] = HarrisDetector( img, scaleD, scaleI, count, fRadius, fExist, cutoff )
% [ features, himg ] = HarrisDetector( img, count, scaleD, scaleI ) - Harris
% interest feature point detector.
% Finds Harris corner features in a given image. If feats is supplied only
% new features not near existing valid features are added to the feature
% list.
%
% Input (default):
% img         - The input image
% scaleD      - The derivative scale at which to detect features (1.0)
% scaleI      - The integration scale over which to sum features (1.0)
% count       - The targeted number of feature points (1000)
% fRadius     - The radius around an established feature where no other
% features should be selected (5)
% fExist      - Existing feature set ([])
% cutoff      - The percentage of maximum below which to ignore features
% (0.05)
%
% Output:
% features - The location of features; a Nx4 array of [y,x,error,valid] points
% himg     - The computed Harris feature image

% Default parameters
if (nargin < 7)
    cutoff = 0.05;
end;
if (nargin < 6)
    fExist = [];
end;
if (nargin < 5)
    fRadius = 5;
end;
if (nargin < 4)
    count = 1000;
end;
if (nargin < 3)
    scaleI = 1.0;
end;
if (nargin < 2)
    scaleD = 1.0;
end;

% Create some Gaussian filters
G = GaussianKernel1D(scaleD, 0, 3);
dG = GaussianKernel1D(scaleD, 1, 3);

% Compute the x and y derivatives of the input image
Ix = filter2(G'*dG, img);
Iy = filter2(dG'*G, img);

% Compute the components of the structure tensor
Ixx = Ix.*Ix;
Iyy = Iy.*Iy;
Ixy = Ix.*Iy;

% Integrate the components of the structure tensor over the integration
% window, weighted with a Gaussian function.
Gi = GaussianKernel1D(scaleI, 0, 3);
GG = Gi'*Gi;
Gxx = filter2(GG, Ixx);
Gyy = filter2(GG, Iyy);
Gxy = filter2(GG, Ixy);

% Harris feature image function
himg = (Gxx.*Gyy - Gxy.*Gxy) - 0.04*(Gxx + Gyy).^2;
hcutoff = max(himg(:)) * cutoff;

% Find local maxima in the Harris feature image
[h, w] = size(img);
M = zeros(h,w);
ii = 1+fRadius:h-fRadius;
jj = 1+fRadius:w-fRadius;
% M marks the possible location of Harris features--local maxima
% (himg(ii,jj)>hcutoff).*
M(ii,jj) = (himg(ii,jj)>hcutoff).*(himg(ii,jj)>himg(ii-1,jj-1)).*(himg(ii,jj)>himg(ii-1,jj)).*(himg(ii,jj)>himg(ii-1,jj+1)).*(himg(ii,jj)>himg(ii,jj-1)).*(himg(ii,jj)>himg(ii,jj+1)).*(himg(ii,jj)>himg(ii+1,jj-1)).*(himg(ii,jj)>himg(ii+1,jj)).*(himg(ii,jj)>himg(ii+1,jj+1));

% Handle existing features
if (~isempty(fExist))  
    % Kill features that have gotten too close to the edge of the image
    fidx = find(fExist(:,4));
    fExist(fidx,4) = (fExist(fidx,1) > 1+fRadius).*(fExist(fidx,1) < h-fRadius).*(fExist(fidx,2) > 1+fRadius).*(fExist(fidx,2) < w-fRadius);

    % Zero out regions in the feature map that are close to existing features
    fidx = find(fExist(:,4));
    for idx=1:length(fidx)
        i = fidx(idx);
        ii = max(1,fExist(i,1)-fRadius):min(h,fExist(i,1)+fRadius);
        jj = max(1,fExist(i,2)-fRadius):min(w,fExist(i,2)+fRadius);
        M(round(ii),round(jj)) = 0;
    end;
end;

% hScore preserves the Harris feature scores only at local maxima
hScore = himg.*M;

% Find indices of non-zero scores, and sort according to score
[si, sj, sVal] = find(hScore);
[tmp, sortIdx] = sort(sVal,1,'descend');

% Copy existing features to result
features = fExist;

% Add new features if they are not too close to existing features
idx = 1;
while ((isempty(features) || sum(features(:,4)) < count) && idx <= length(sortIdx))
    % Get the feature's position
    pos = [si(sortIdx(idx)) sj(sortIdx(idx))];
    % Check if feature is still valid
    if (M(pos(1),pos(2)) > 0)
        features(end+1,:) = [(pos) 0 1];
        % clear the neigborhood
        ii = max(1,features(end,1)-fRadius):min(h,features(end,1)+fRadius);
        jj = max(1,features(end,2)-fRadius):min(w, features(end,2)+fRadius);
        M(ii,jj) = 0;
    end;
    idx = idx+1;
end;
