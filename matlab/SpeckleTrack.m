% read images
path = '..\..\data\Lisa\out\TestGUI\';
images = inputImages(path,'Reg-GoodfromWHrh0*.png');
% path = '..\..\data\test\Quenot\';
% images = inputImages(path,'Ice*.png');

% detect features
levels = 8;
maxCount = 500;
initScale = 0.3;
hlF = [];
for i=1:size(images,1)
    hlF = cat(1, hlF, struct('features', findHLFeatures(images(i).img, levels, maxCount, initScale)));
end;

% find matches
radius = 10;
minCorr = 0.65;
hlM = [];
for i=1:size(hlF,1)-1
    disp(strcat('matching...', num2str(i), '->', num2str(i+1)));
    hlM = cat(1, hlM, struct('matches', computeMatches(images(i).img, hlF(i).features, images(i+1).img, hlF(i+1).features, radius, minCorr)));
end;

% show tracks
mov = makeTrackMovie(images, hlM);
% movie2avi(mov, strcat(path, 'IceTrack2.avi'), 'COMPRESSION', 'None');
movie(mov, 10, 2);