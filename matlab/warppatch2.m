% IP=warppatch(I,x,A,r)
%
% I = image (hxw)
% x = coordinates of point [y;x]
% A = 2x2 affine transformation
% r = radius of region
%
% Note: matlab uses vertical coordinate first, then horizontal, i.e. use m=[y; x] for points.
%       The same is true for transformation.

function IP=warppatch(I,x,A,r)

[h,w]=size(I);
xcorner=x-[r;r];
A=[A -A*x+[r;r]; 0 0 1]; %matlab defines transposed transformations
T=maketform('affine',A');

resamp=makeresampler('cubic','bound');
IP=tformarray(I,T,resamp,[1 2],[1 2],[2*r+1 2*r+1],[],[]);

% size(IP);
% imshow(IP);