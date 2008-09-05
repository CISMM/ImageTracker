function [model, inliers] = RansacRigid(ptsA, ptsB, tolerance, inlierPct, accuracy)

N = size(ptsA, 2);

% Convert data points to homogeneous coordinates and combine into one data
% matrix.  A gets the first three rows, B gets the second three rows.
data = ...
    [ptsA
     ones(1,N)
     ptsB
     ones(1,N)];
 
[model, inliers] = Ransac(data, @CreateModel, @Distance, 3, tolerance, inlierPct, accuracy);

function [model] = CreateModel(data)

% find number of points in data set
N = size(data, 2);

% linear regression: find the transform that maps points in B to points in
% A.  Mx = A
M = ...
    [data(4:6, :)'  zeros(N, 3)
     zeros(N, 3)    data(4:6, :)'];
b = [data(1, :)'
     data(2, :)'];
if (rank(M) < 6)
    model = eye(3);
else
    x = M\b;

    % reshape parameters into transform
    model = [reshape(x, 3, 2)'; 0 0 1];
end

function [distance] = Distance(data, model)
A = data(1:3,:);
B = model*data(4:6,:);
distance = sqrt(sum((A - B).^2, 1));
