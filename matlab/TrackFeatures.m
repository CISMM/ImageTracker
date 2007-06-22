function [ d ] = TrackFeatures( dx, dy, dt, pts, radius )
% TrackFeatures(dx, dy, dt, pts, radius) Tracks features from one frame to
% the next.
% Given the derivatives of an image sequence in x, y, and t and the indices
% of features to track, this function finds the displacement of the
% features.
% 
% Input (default)
% dx          - The x-derivative of the image sequence
% dy          - The y-derivative of the image sequence
% dt          - The t-derivative of the image sequence
% pts         - The indices (y,x) of the interest features
% radius      - The image patch radius to use when computing the
% displacement (2)
%
% Output
% d           - The displacement of each image feature in pts (y, x)

if (nargin < 5)
    radius = 2;
end;

% Create a weighting function--the error in the tracker increases from the
% center of the image patch
% TODO: the weighting seems to kill the tracking...what gives, Trucco?
% Gs = radius / 2;
% g = GaussianKernel1D(Gs, 0, 2);
% gg = g'*g;
% gg = gg/max(gg(:));

[h,w] = size(dx);

d = zeros(size(pts,1),2);
for i=1:size(pts,1)
    if (pts(i,4) == 0) 
        % Feature is not valid, ignore
    else
        y = pts(i,1);
        x = pts(i,2);
        yy = max(1,y-radius):1.0:min(h,y+radius);
        xx = max(1,x-radius):1.0:min(w,x+radius);

        % Find the indices for the weighting function
    %     wxi = radius+1 - min(radius,x):radius+1 + min(radius,w-x);
    %     wyi = radius+1 - min(radius,y):radius+1 + min(radius,h-y);
    %     w = [gg(wyi,wxi)];
    %     ww = [w(:) w(:)].^2';

        % create the sampling grid
        [xidx, yidx] = meshgrid(xx,yy);

        % interpolate derivatives on the sampling grid
        dxx = interp2(dx, xidx, yidx);
        dyy = interp2(dy, xidx, yidx);
        dtt = interp2(dt, xidx, yidx);

        A = [dxx(:), dyy(:)];
        b = -dtt(:);
    %     v = inv((A'.*ww)*A)*A'.*ww*b;
        v = inv(A'*A)*A'*b;
        d(i,:) = [v(2) v(1)]; % have to swap order of x,y displacement
    end;
end;
