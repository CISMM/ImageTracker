% load image data
img = double(rgb2gray(imread('davidbowie.jpg')));

% create a gaussian filter
g = GaussianKernel1D(1.0, 0, 3);

% create noise image with std 25, all positive
% img = randn(512,512);
% img = filter2(g, img, 'same');
% img = filter2(g', img, 'same');
% img = 25*img;
% img = img - min(img(:));

% create an image sequence
imgs = TransformImage( img, [256 256], [50 50], [0 0], 0, [-10 -10], 10);
% add noise
% imgs = imgs + 0.05*(max(imgs(:))-min(imgs(:)))*randn(size(imgs));
% display
ShowMovie(imgs,5);

[h, w, t] = size(imgs);

searchR = [10 10];
templateR = [5 5];

% create an initial feature list
% yy = searchR(1)+1:h-searchR(1);
% xx = searchR(2)+1:w-searchR(1);
% [xxg, yyg] = meshgrid(xx,yy);
% i = 1:100:length(yy)*length(xx);
% feat = [ yyg(i); xxg(i); ones(2, length(i)) ]';

[feat, himg] = HarrisDetector(squeeze(imgs(:,:,1)), 1.0, 1.0, 1000, searchR(1), [], 0.001);
feat(:,:,2) = 0; % hack to add an extra dimension to the feature set.

meanFeat = ExtractFeatureImages(squeeze(imgs(:,:,1)), feat, templateR);

for f = 2:t
    [feat, meanFeat] = NCCFlow(imgs, f, templateR, searchR, feat, 0.90, meanFeat);
end;

ShowFeatureMovie(imgs, feat, 1);
figure; ShowMovie(meanFeat, 4);