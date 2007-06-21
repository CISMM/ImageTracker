function [ features ] = KLTTracker( imgs, sigmaS, sigmaT, count, featRadius, featFrame )
%KLTTRACKER Summary of this function goes here
%   Detailed explanation goes here

display(sprintf('KLTTracker: %s \t Starting', datestr(now, 'HH:MM:SS')));

if (nargin < 6)
    featFrame = 5;
end;
if (nargin < 5)
    featRadius = 5;
end;
if (nargin < 4)
    count = 500;
end;
if (nargin < 3)
    sigmaT = 1.0;
end;
if (nargin < 2)
    sigmaS = 1.0;
end;

[h w t] = size(imgs);

% Create some 1D Gaussian kernels
Gs = GaussianKernel1D(sigmaS, 0, 3);
Gt = GaussianKernel1D(sigmaT, 0, 3);
% We negate the derivative kernel to detect positive step edges when
% filtering (equivalent to convolution with true derivatives)
dGs = -GaussianKernel1D(sigmaS, 1, 3);
dGt = -GaussianKernel1D(sigmaT, 1, 3);

% Create some spatial kernels
Gx = Gs'*dGs;
Gy = dGs'*Gs;

% Find initial features
features(:,:,1) = HarrisDetector(imgs(:,:,1), sigmaS, sigmaS, count, featRadius);
displayFeatures(imgs(:,:,1), features(:,:,1));

for i=1:t-1
    % Find new features, if need be
    if (i > 1 && mod(i-1,featFrame) == 0)
        newFeat = HarrisDetector(imgs(:,:,i), sigmaS, sigmaS, count, featRadius, features(:,:,i));
        features(end:size(newFeat,1),:,:) = 0; % expand features
        features(:,:,i) = newFeat;
        clear newFeat;
    end;
    
    Ix = filter2(Gx, imgs(:,:,i));
    Iy = filter2(Gy, imgs(:,:,i));
    It = imgs(:,:,i+1)-imgs(:,:,i);
    d = TrackFeatures(Ix, Iy, It, features(:,:,i));
    features(:,1:2,i+1) = features(:,1:2,i) + d;
    [err, val] = CheckError(imgs, features);
    features(:,3:4,i+1) = [err val];
    
    % Progress update
    display(sprintf('Progress %s: %3d of %3d (%3d%%), %5d of %5d features valid', datestr(now, 'HH:MM:SS'), i, t-1, round(100*i/(t-1)), sum(features(:,4,i+1)), size(features,1)));
end;
display(sprintf('KLTTracker: %s \t Finished', datestr(now, 'HH:MM:SS')));
