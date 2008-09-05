function [ features ] = KLTTrackerStep( imgs, frames, features, params )
% KLTTrackerStep Tracks features from one frame to another.
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

pFrame = frames(1);
cFrame = frames(2);
I1 = imgs(:,:,pFrame);
I2 = imgs(:,:,cFrame);

% Create some 1D Gaussian kernels
Gs = GaussianKernel1D(params.derivativeScale, 0, 3);
% We negate the derivative kernel to detect positive step edges when
% filtering (equivalent to convolution with true derivatives)
dGs = -GaussianKernel1D(params.derivativeScale, 1, 3);
Gi = GaussianKernel1D(params.integralScale, 0, 3);

% Create some spatial kernels
Gx = Gs'*dGs;
Gy = dGs'*Gs;
Gii = Gi'*Gi;

% compute image gradients
I = I1 + I2;
Ix = filter2(Gx, I);
Iy = filter2(Gy, I);
Ixx = Ix.*Ix;
Iyy = Iy.*Iy;
Ixy = Ix.*Iy;

% smooth
% Ixx = filter2(Gii, Ixx);
% Iyy = filter2(Gii, Iyy);
% Ixy = filter2(Gii, Ixy);

% Create a weighting function--the error in the tracker increases from the
% center of the image patch
% TODO: the weighting seems to kill the tracking...what gives, Trucco?
% Gs = radius / 2;
% g = GaussianKernel1D(Gs, 0, 2);
% gg = g'*g;
% gg = gg/max(gg(:));

[h,w] = size(I1);
N = length(features);

for idx=1:N     % consider each feature
    if (features(idx).active(pFrame))    % only work on active features
        x = features(idx).pos(pFrame, 2);
        y = features(idx).pos(pFrame, 1);
        
        % exclude features that have search windows that run off the edge
        % of the frame
        if (y - params.searchRadius(1) < 1 || ...
            y + params.searchRadius(1) > h || ...
            x - params.searchRadius(2) < 1 || ...
            x + params.searchRadius(2) > w)
                features(idx).pos(cFrame,:) = [0 0];
                features(idx).error(cFrame) = 0;
                features(idx).active(cFrame) = 0;
                continue;
        end    

        % Find the indices of the image patch
        yy = y - params.featureRadius(1) : 1.0 : y + params.featureRadius(1);
        xx = x - params.featureRadius(2): 1.0 : x + params.featureRadius(2);

        % create the sampling grid over the image patch
        [xidx, yidx] = meshgrid(xx,yy);

        % interpolate patch derivatives on the sampling grid
        Pxx = interp2(Ixx, xidx, yidx);
        Pyy = interp2(Iyy, xidx, yidx);
        Pxy = interp2(Ixy, xidx, yidx);
        Px = interp2(Ix, xidx, yidx);
        Py = interp2(Iy, xidx, yidx);
        Pxx = Pxx(:);
        Pyy = Pyy(:);
        Pxy = Pxy(:);
        Px = Px(:);
        Py = Py(:);
        
        d = [0; 0];

        for iter = 1:params.iterations
            % Each iteration, the estimate of the new image patch changes, so
            % the estimate of It changes. Here, we use the current displacement
            % estimate to define the image patch in the second frame.
            xx2 = xx + d(1);
            yy2 = yy + d(2);

            [xidx2 yidx2] = meshgrid(xx2, yy2);
            Pt = interp2(I2, xidx2, yidx2, 'linear', 0) - interp2(I1, xidx, yidx, 'linear', 0);
            Pt = Pt(:);

            A = [Pxx Pxy;
                 Pxy Pyy];
            b = -([Pt.*Px; Pt.*Py]);
            
            v = A\b;

            d = d + v; % update displacement
        end;
        
        % update positions
        features(idx).pos(cFrame, 2) = x + d(1);
        features(idx).pos(cFrame, 1) = y + d(2);
        
        % Check error--compare new feature patch with current patch
        x = features(idx).pos(cFrame, 2);
        y = features(idx).pos(cFrame, 1);
        yy = y - params.featureRadius(1) : 1.0 : y + params.featureRadius(1);
        xx = x - params.featureRadius(2) : 1.0 : x + params.featureRadius(2);
        [xidx, yidx] = meshgrid(xx, yy);
        newPatch = interp2(I2, xidx, yidx, 'linear', 0);
        
        features(idx).error(cFrame) = ncc(newPatch, features(idx).patch);
        features(idx).active(cFrame) = features(idx).error(cFrame) > params.errorTolerance;
        
        % update feature patch if necessary
        if (params.updateFeature)
            features(idx).patch = newPatch;
        end
    else    % lost features    
        features(idx).pos(cFrame,:) = [0 0];
        features(idx).error(cFrame) = 0;
        features(idx).active(cFrame) = 0;
    end
end
