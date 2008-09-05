%% Optical Flow
% Tests multiresolution optical flow methods.

%%
clear;
close all;
format compact;

%% Random dots
% To generate images with high texture components, we randomly splat
% Gaussian dots onto the image plane.  The test videos are known transforms
% and warpings of these random dot images.

% create image of random Gaussian splats
g = GaussianKernel1D(1.0, 0, 3);
gg = g'*g;
img = RandomSplat([1024 1024], 0.94, gg);
img = 255*(img-min(img(:)))/range(img(:));
dispimg(img);

%% Test cases
% The test case data is held in a structure that defines all parameters of
% test cases.  This makes it easy to handle and pass the test information.

% Initial test settings
test.method = 'clg';
test.sigmaD = 0.8;
test.sigmaI = 4.0;
test.regular = 100;
test.iters = 100;
test.levels = 4;

%% Translation
% *Pure x-translation*
% 
% We generate an image sequence over which the translation between two
% consecutive images increases from very little displacement per frame to a
% large displacement.

imgSize = [256 256];
imgOffset = [256 256];
test.vary.data = [0.1 0.25:0.5:5];
test.vary.label = 'Displacement';
test.vary.units = 'px';

translate = [zeros(1,size(test.vary.data,2)); cumsum(test.vary.data)];
N = size(translate,2);
test.imgs = zeros(imgSize(1), imgSize(2), N);
test.dx = zeros(imgSize(1), imgSize(2), N);
test.dy = zeros(imgSize(1), imgSize(2), N);
for (i = 1:N)
    tform = CreateTransform([128 128], [1 1], 0, translate(:,i));
    [test.imgs(:,:,i) test.dx(:,:,i) test.dy(:,:,i)] = TransformImage(img, imgSize, imgOffset, tform);
end
figure(1); clf;
dispimg(ImageMontage(test.imgs, 7, 5, 0));
drawnow;
ShowMovie(test.imgs);

%%
TestcaseOpticalFlow;

%% Rotation
imgSize = [256 256];
imgOffset = [256 256];
test.vary.data = [0.001:0.001:0.02];
test.vary.label = 'Angle of Rotation';
test.vary.units = '\pi rad';

rotate = cumsum(test.vary.data) * pi;
N = size(test.vary.data, 2);
test.imgs = zeros(imgSize(1), imgSize(2), N);
test.dx = zeros(imgSize(1), imgSize(2), N);
test.dy = zeros(imgSize(1), imgSize(2), N);
for (i = 1:N)
    tform = CreateTransform([128 128] + imgOffset, [1 1], rotate(i), [0 0]);
    [test.imgs(:,:,i) test.dx(:,:,i) test.dy(:,:,i)] = TransformImage(img, imgSize, imgOffset, tform);
end
figure(1); clf;
dispimg(ImageMontage(test.imgs, 7, 5, 0));
ShowMovie(test.imgs);

%%
TestcaseOpticalFlow;

%%

imgs = LoadImages('/home/beastwoo/Nano/data/falvo/fibrin-sheet-070612/sheet-%04d.tif', 100, 105);
figure(1); clf;
ShowMovie(imgs);

%%
figure(1); clf;

% preallocate output data
[h, w, t] = size(imgs);
flow = zeros(h, w, 2, t-1);
rmse = zeros(t-1,1);
psnr = zeros(t-1,1);

% flow computation parameters
test.sigmaD = 1.0;
test.sigmaI = 4.5;
test.regular = 10000;
test.iters = 200;
test.levels = 5;
errThresh = 5000;

flowFcn = @(img1, img2) CLGOpticalFlow(img1, img2, test.sigmaD, test.sigmaI, test.regular, 1.9, test.iters);

tic;
for i = 1:t-1
    % flow computation for one frame pair
    [ui, vi] = MultiResOpticalFlow(imgs(:,:,i), imgs(:,:,i+1), test.levels, flowFcn);
    
    % check optical flow error
    [rmse(i), psnr(i), errImg] = CompareImages(imgs(:,:,i), WarpImage(imgs(:,:,i+1), ui, vi), 12);
    sqrtErr = sqrt(errImg);

    % mask out high-error regions
%     mask = sqrtErr > errThresh;
%     ui(mask) = 0;
%     vi(mask) = 0;
    flow(:,:,:,i) = cat(3, ui, vi);
    
    % display flow and error
    figure(1); clf;
    subplot(1,2,1);
    ShowVectorField(flow(:,:,:,i), 1000, 20, imgs(:,:,i));
    subplot(1,2,2);
    dispimg(sqrtErr);
    drawnow;
end
elapsedTime = toc;
fprintf('%d frames in %0.4fs, %0.4fs/f\n', i, elapsedTime, elapsedTime/(i));