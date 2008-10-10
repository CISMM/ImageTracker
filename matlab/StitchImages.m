function [imgMosaic] = StitchImages(imgs, disps)
% StitchImages - creat an image mosaic from a set of images and
% displacements.
%
% Given a set of images and displacements (relative to the first image),
% create an image mosaic by stitching together all images.  The result is
% a larger image that includes image intensities from all the constituent
% images.
%
% Input
%   imgs -      the images to stitch together
%   disps -     the displacements that relate how the images relate to each
%   other spatially
%
% Output
%   imgMosaic - the resulting image mosaic

[h, w, t] = size(imgs);
minD = min(disps);
maxD = max(disps);
min_0 = 1 - maxD;
max_0 = [w h] - minD;

mosaicSize = ceil(max_0 - min_0);

imgMosaic = zeros(mosaicSize(2), mosaicSize(1));

for i = 1:t
    coordWorld1 = ceil(1 + maxD - disps(i,:));
    coordWorld2 = floor([w h] + maxD - disps(i,:));
    coordImg1 = coordWorld1 - maxD + disps(i,:);
    coordImg2 = coordWorld2 - maxD + disps(i,:);
    [gridX, gridY] = meshgrid(coordImg1(1):coordImg2(1), coordImg1(2):coordImg2(2));
    imgMosaic(coordWorld1(2):coordWorld2(2), coordWorld1(1):coordWorld2(1)) = interp2(imgs(:,:,i), gridX, gridY, 'linear', 0);
    dispimg(imgMosaic);
    drawnow;
%     saveas(gca, sprintf('imgs/stitch-%04d.tif', i));
end
