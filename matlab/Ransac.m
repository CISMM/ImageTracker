function [model, inliers] = Ransac(data, modelFunc, distanceFunc, minModel, tolerance, inlierPct, accuracy)
% Ransac - generic RANSAC implementation.
%
% Input:
%   data -      the data to evaluate, an MxN matrix where M is the number
%   of elements per point and N is the number of data points
%   modelFunc(data) : model - a function handle that returns a model
%   given at least minModel points from the data 
%   distanceFunc(data, model) : error - a function handle that evaluates
%   the error for all data points given a model
%   minModel -  the minimum number of points required to establish a model
%   tolerance - the error tolerance for point acceptance
%   inlierPct - the estimated percentage of inliers (0 .. 1)
%   accuracy -  the desired percent that the found model is correct (0 ..
%   1)
%
% Output:
%   model -     the best model found
%   inliers -   a logical array indicating inlier indices

[M, N] = size(data);

% determine number of tries to make
maxTries = ceil(log(1 - accuracy) / log(1 - inlierPct^minModel));

% initialize best solution
model = [];
inliers = [];
count = 0;
tries = 0;

while tries < maxTries
    tries = tries + 1;
    
    % randomly pick a set of indices
    tryIds = zeros(1, minModel);
    while (length(unique(tryIds)) ~= minModel | sum(tryIds) == 0)
        tryIds = ceil(N * rand(1, minModel));
    end
    
    lastIds = [];
    refineCount = 0;
    
    % refine model until it is stable
    while (~isequal(tryIds, lastIds) & refineCount < 30)
        lastIds = tryIds;
        refineCount = refineCount + 1;
    
        % make a model
        tryModel = modelFunc(data(:,tryIds));

        % find other points that agree with this model
        tryDist = distanceFunc(data, tryModel);
        tryIds = tryDist <= tolerance;    
    end
    
    % update our model if it is the best seen so far
    if (sum(tryIds) > count)
        model = tryModel;
        inliers = tryIds;
        count = sum(tryIds);
        
        % update our number of attempts based on how many inliers we found
        inlierPct = count / N;
        maxTries = ceil(log(1 - accuracy) / log(1 - inlierPct^minModel));
    end
end

global debug;
if (isempty(debug))
    debug = false;
end
if (debug)
    fprintf('RANSAC: Best transform found in %d tries with %0.3f %% inliers.\n', tries, (100 * count / N));
end