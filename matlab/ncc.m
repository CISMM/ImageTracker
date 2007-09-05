function [ n ] = ncc(imgX, imgY)
% n = ncc(imgX, imgY) Computes the normalized cross-correlation
% between two input images.  The images must be of the same
% dimensions.

n = 0;
if (size(imgX) ~= size(imgY))
    disp('Image patch sizes do not match.');
    return;
end;

Ix = imgX(:);
Iy = imgY(:);

Ix = Ix - mean(Ix);
Iy = Iy - mean(Iy);
Ix = Ix/norm(Ix);
Iy = Iy/norm(Iy);

n = Ix'*Iy;

