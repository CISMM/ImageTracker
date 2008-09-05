function [windImg, fmap] = WindowImage(img, padMethod, radius, func, val)

if (nargin < 2)
    padMethod = 'replicate';
end
if (nargin < 3)
    radius = ceil(0.5*size(img,1));
end
if (nargin < 4)
    % hanning window
    func = @(x) 0.5*(1 - cos(x*pi));
end
if (nargin < 5)
    val = mean(img(:));
end

if (~strcmp(padMethod, 'none'))
    windImg = padarray(img, [radius radius], padMethod);
else
    windImg = img;
end

[h, w] = size(windImg);

% create a map of the distance between each pixel and the edge of the image
xx = 1:w;
yy = 1:h;
dx = min(xx-1, w-xx);
dy = min(yy-1, h-yy);

% flatten center
dx(dx > radius) = radius;
dy(dy > radius) = radius;

% scale distance between zero and 1
dx = dx / radius;
dy = dy / radius;

% compute the windowing function in each direction
funcx = func(dx);
funcy = func(dy);

% compute whole window
fmap = funcy' * funcx;

% apply the window
windImg = windImg .* fmap + (1-fmap)*val;
