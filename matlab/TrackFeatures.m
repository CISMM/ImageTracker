function [ d ] = TrackFeatures( I1, I2, pts, radius, sigmaS, iters )
% TrackFeatures(GI1, GI2, dt, pts, radius) Tracks features from one frame to
% the next.
% Given the derivatives of an image sequence in x, y, and t and the indices
% of features to track, this function finds the displacement of the
% features.
% 
% Input (default)
% GI          - The spatial gradient of the image sequence at the current
% frame (or between the two frames)
% dt          - The t-derivative of the image sequence
% pts         - The indices (y,x) of the interest features
% radius      - The image patch radius to use when computing the
% displacement (2)
%
% Output
% d           - The displacement of each image feature in pts (y, x)

if (nargin < 6)
    iters = 5;
end;
if (nargin < 5)
    sigmaS = 1.0;
end;
if (nargin < 4)
    radius = 2;
end;

% Create some 1D Gaussian kernels
Gs = GaussianKernel1D(sigmaS, 0, 3);
% We negate the derivative kernel to detect positive step edges when
% filtering (equivalent to convolution with true derivatives)
dGs = -GaussianKernel1D(sigmaS, 1, 3);

% Create some spatial kernels
Gx = Gs'*dGs;
Gy = dGs'*Gs;

% compute image gradients
I = I1 + I2;
GI(:,:,1) = filter2(Gx, I);
GI(:,:,2) = filter2(Gy, I);

% Create a weighting function--the error in the tracker increases from the
% center of the image patch
% TODO: the weighting seems to kill the tracking...what gives, Trucco?
% Gs = radius / 2;
% g = GaussianKernel1D(Gs, 0, 2);
% gg = g'*g;
% gg = gg/max(gg(:));

[h,w] = size(I1);

d = zeros(size(pts,1),2);

% Find valid feature indices
fidx = find(pts(:,4));

for idx=1:length(fidx) % for each valid feature
    i = fidx(idx);
    y = pts(i,1);
    x = pts(i,2);
    
    % Find the indices of the image patch
    yy = max(1,y-radius):1.0:min(h,y+radius);
    xx = max(1,x-radius):1.0:min(w,x+radius);
    
    % create the sampling grid over the image patch
    [xidx, yidx] = meshgrid(xx,yy);

    % interpolate derivatives on the sampling grid, using the symmetric
    % treatment of the image gradients
    dxx = interp2(GI(:,:,1), xidx, yidx);
    dyy = interp2(GI(:,:,2), xidx, yidx);
    
    for iter = 1:iters
        % Each iteration, the estimate of the new image patch changes, so
        % the estimate of It changes. Here, we use the current displacement
        % estimate to define the image patch in the second frame.
        yy2 = yy + d(i,1);
        xx2 = xx + d(i,2);
        [xidx2 yidx2] = meshgrid(xx2, yy2);
        dtt = interp2(I2, xidx2, yidx2) - interp2(I1, xidx, yidx);

        A = [dxx(:), dyy(:)];
        b = -dtt(:);
        v = A\b;

        d(i,:) = d(i,:) + [v(2) v(1)]; % have to swap order of x,y displacement
    end;
end;
