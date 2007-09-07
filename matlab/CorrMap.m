function [ map ] = CorrMap( img, template )

[h,w] = size(template);
map = zeros(size(img));

% find the middle index of the template in each direction
mtx = ceil(w/2);
mty = ceil(h/2);

for j = mty:size(img,1)-mty
    for i = mtx:size(img,2)-mtx
        xx = i - mtx + 1 : i - mtx + w;
        yy = j - mty + 1 : j - mty + h;
        map(j,i) = ncc(img(yy, xx),template);
    end;
end;
