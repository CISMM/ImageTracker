searchR = [15 15];
templateR = [6 6];
featThreshold = 0.01;
errorTol = 0.00;
evolve = 0;

newImages = 1;
pctNoise = 0.00;
translate = [10, -10];
rotate = 0 * pi / 180;
steps = 10;

maxFeatures = 30;

if (newImages)
    % David Bowie
    % load image data
%    img = double(imread('/home/beastwoo/Nano/data/vinay/cell03-0080.tif'));
    img = double(rgb2gray(imread('davidbowie.jpg')));

    % Random images
    % create a gaussian filter
    g = GaussianKernel1D(1.0, 0, 3);

    % create noise image with std 25, all positive
    % img = randn(512,512);
    % img = filter2(g, img, 'same');
    % img = filter2(g', img, 'same');
    % img = 25*img;
    % img = img - min(img(:));

    % create an image sequence
    imgs = TransformImage( img, [256 256], [50 50], [178 178], rotate, translate, steps);
    
    % add noise
    imgs = imgs + pctNoise*(max(imgs(:))-min(imgs(:)))*randn(size(imgs));
end;

% display
figure; ShowMovie(imgs);

[h, w, t] = size(imgs);

% create an initial feature list
% yy = searchR(1)+1:h-searchR(1);
% xx = searchR(2)+1:w-searchR(1);
% [xxg, yyg] = meshgrid(xx,yy);
% i = 1:100:length(yy)*length(xx);
% feat = [ yyg(i); xxg(i); ones(2, length(i)) ]';

[feat, himg] = HarrisDetector(squeeze(imgs(:,:,1)), 1.0, 1.0, 30, searchR(1), [], featThreshold);
feat(:,:,2) = 0; % hack to add an extra dimension to the feature set.

meanFeat = ExtractFeatureImages(squeeze(imgs(:,:,1)), feat, templateR);
figure; dispimg(ImageMontage(meanFeat,10,1,0));

for f = 2:t
    [feat, meanFeat] = NCCFlow(imgs, f, templateR, searchR, feat, errorTol, meanFeat, evolve);
end;

figure; dispimg(ImageMontage(meanFeat,10,1,0));

% Plot tracking error
disp(sprintf('%d of %d (%% %3.2f) features tracked successfully', sum(feat(:,4,end)), size(feat,1), (100 * sum(feat(:,4,end)) / size(feat,1))));
figure;
hold on;
for i=1:size(feat,1)
    plot(squeeze(feat(i,3,:)));
end;
hold off;

figure; ShowFeatureMovie(imgs, feat, 1);
