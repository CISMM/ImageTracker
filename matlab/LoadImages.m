function [ imgs ] = LoadImages( format, start, stop, stride, rgb )
% LoadImages(format, start, stop) Loads a sequence of images.
% Loads the image sequence (video) specified by the format and start and
% stop indices.  The image sequence is returned as a NxMxF matrix, where
% each frame is NxM (h,w) pixels and F=stop-start+1.  Each pixel is
% represented as a double grayscale value.
%
% Inputs (default):
% format   - The sprintf format of the image sequence file names, e.g.
% 'myseq-%04d.tif' for myseq-0000.tif, myseq-0001.tif, etc.
% start    - The starting image index of the file sequence
% stop     - The stopping image index of the file sequence
% stride   - The distance between consecutive image indices (1)
% rgb      - 1 if the images are rgb format; they will be converted to
% grayscale (0)
%
% Output:
% imgs     - The NxMxF image sequence data

if (nargin < 5)
    rgb = 0;
end;
if (nargin < 4)
    stride = 1;
end;


idx=1;
ii = start:stride:stop;
t = length(ii);

if (rgb == 0)
    % read first image to get dimensions
    img = double(imread(sprintf(format,start)));
    [h w] = size(img);
    imgs = zeros(h,w,t);
    clear img;
    
    for i=start:stride:stop
        imgs(:,:,idx) = double(imread(sprintf(format, i)));
        idx = idx+1;
    end;
else % rgb images
    % read first image to get dimensions
    img = double(rgb2gray(imread(sprintf(format,start))));
    [h w] = size(img);
    imgs = zeros(h,w,t);
    clear img;
    
    for i=start:stride:stop
        imgs(:,:,idx) = double(rgb2gray(imread(sprintf(format, i))));
        idx = idx+1;
    end;
end;    