newImages = 1;
multilayer = 1;
pctNoise = 0.00;
steps = 15;
translate = 3*[steps, -steps];
rotate = 0 * pi / 180;

searchR = [10 10];
templateR = searchR/2;
featThreshold = 0.00;
errorTol = 0.45;
evolve = 1;

maxFeatures = 100;

if (newImages)
    % create a gaussian filter
    g = GaussianKernel1D(2.0, 0, 3);
    
    % David Bowie
    % load image data
    % img1 = double(rgb2gray(imread('davidbowie.jpg')));
    % img = double(imread('/home/beastwoo/Nano/data/vinay/cell03-0080.tif'));
    img1 = double(imread('/home/beastwoo/Nano/data/cribb/cilia-cult02/cult02-000000.tif'));

%     img1 = randn(512,512);
%     img1 = filter2(g'*g, img1, 'same');
%     img1 = 200 * img1;
%     img1 = img1 - min(img1(:));

    % create noise image
    img2 = randn(512,512);
    img2 = filter2(g'*g, img2, 'same');
    img2 = 200 * img2;
    img2 = img2 - min(img2(:));

    % create an image sequence
    imgs1 = TransformImage(img1, [256 256], [50 150], [242 242], rotate, translate, steps);
    imgs2 = TransformImage(img1, [256 256], [150 50], [242 242], -rotate, -translate, steps);
    
    % combine?-
    if (multilayer)
        imgs = imgs1 + imgs2;
    else
        imgs = imgs1;
    end;
    
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

[feat, himg] = HarrisDetector(squeeze(imgs(:,:,1)), 1.0, 1.0, maxFeatures, searchR(1), [], featThreshold);
feat(:,:,2) = 0; % hack to add an extra dimension to the feature set.

meanFeat = ExtractFeatureImages(squeeze(imgs(:,:,1)), feat, templateR);
figure; dispimg(ImageMontage(meanFeat,10,1,0)); pause(0.1);

for f = 2:t
    [feat, meanFeat, corrMaps] = NCCFlow(imgs, f, templateR, searchR, feat, errorTol, meanFeat, evolve);
    figure(10); dispimg(ImageMontage(corrMaps, 10, 1, 0)); pause(0.1);
end;

figure; dispimg(ImageMontage(meanFeat,10,1,0)); pause(0.1);

% Plot tracking error
disp(sprintf('%d of %d (%% %3.2f) features tracked successfully', sum(feat(:,4,end)), size(feat,1), (100 * sum(feat(:,4,end)) / size(feat,1))));
figure;
hold on;
for i=1:size(feat,1)
    plot(squeeze(feat(i,3,2:end)));
end;
hold off;
pause(0.1);

figure; ShowFeatureMovie(imgs, feat, 1);
