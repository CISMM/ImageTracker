function [model, inliers] = RansacTranslate(ptsA, ptsB, tolerance, inlierPct, accuracy)

data = [ptsA; ptsB];
[model, inliers] = Ransac(data, @CreateModel, @Distance, 1, tolerance, inlierPct, accuracy);

function [model] = CreateModel(data)
% for translation, the model is just the mean of all displacements in x and
% y, independently
model = mean(data(1:2,:) - data(3:4,:),2);

function [distance] = Distance(data, model)
% find the distance from the model displacement for each point pair
N = size(data,2);
models = repmat(model,1,N);
disp = data(1:2,:) - data(3:4,:);
distance = sqrt(sum((disp - models).^2, 1));