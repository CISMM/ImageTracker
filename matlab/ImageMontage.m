function [ montage ] = ImageMontage( imgs, cols, pad, white )
% ImageMontage(imgs, cols, pad, white) - Creates a montage of all the images in
% the given image sequence
% Inputs
%   imgs        - The image sequence, nxmxF
%   cols        - The number of columns to use in the montage (4)
%   pad         - The padding to apply around each image [y x] ([1 1]); if
%   a single value is supplied, the same padding is used for x and y
%   white       - 1 for a white background, 0 for a black background (1)
%
% Outputs
%   montage     - The image montage

[h, w, f] = size(imgs);

if (nargin < 4)
    white = 1;
end;
if (nargin < 3)
    pad = [1,1];
end;
if (length(pad) == 1)
    pad = pad * [1 1];
end;
if (nargin < 2)
    cols = 4;
end;

rows = ceil(f/cols);

if (white)
    background = max(imgs(:));
else
    background = min(imgs(:));
end;

montage = background * ones(h*rows + pad(1)*(rows+1), w*cols + pad(2)*(cols+1));

for j = 1:rows
    for i = 1:cols
        idx = (j-1)*cols + i;
        if (idx > f)
            break;
        end;
        x = 1 + pad(2) + (i-1)*(w+pad(2));
        y = 1 + pad(1) + (j-1)*(h+pad(1));
        montage(y:y+h-1, x:x+w-1) = imgs(:,:,idx);
    end;
end;
