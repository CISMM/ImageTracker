function [ map ] = CorrMap( img, template )

[h,w] = size(template);
map = zeros(size(img));

% It doesn't seem like normalization can take place over the whole image.
% So, the normalized cross correlation is invariant to local changes in
% image brightness?  But, does this also translate to global changes?
% normalize
% img = img - mean(img(:));
% img = img / norm(img(:));
% template = template - mean(template(:));
% template = template / norm(template(:));

% find the middle index of the template in each direction
mtx = ceil(w/2);
mty = ceil(h/2);

for j = mty:size(img,1)-mty
    for i = mtx:size(img,2)-mtx
        xx = i - mtx + 1 : i - mtx + w;
        yy = j - mty + 1 : j - mty + h;
        imgt = img(yy,xx);
%         map(j,i) = imgt(:)'*template(:);
        map(j,i) = ncc(img(yy, xx),template);
    end;
end;
