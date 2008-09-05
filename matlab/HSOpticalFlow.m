function [ u, v, imgw, res, err ] = HSOpticalFlow( img1, img2, sigmaDeriv, regular, iterations )
% HSOpticalFlow Computes the optical flow between two images in the mannter
% of Horn and Schunck

% Set up default parameters
if (nargin < 5)
    iterations = 200;
end;
if (nargin < 4)
    regular = 500;
end;
if (nargin < 3)
    sigmaDeriv = 1.0;
end;

[h, w] = size(img1);

% Create differentiation kernels
[Gs, rad] = GaussianKernel1D(sigmaDeriv, 0, 3);
dGs = -GaussianKernel1D(sigmaDeriv, 1, 3);

% Compute the image derivatives
% dG = 1/2 * [-1 0 1]; rad = 1;
% Ix = filter2(dG, img1);
% Iy = filter2(dG', img1);
Ix = filter2(Gs'*dGs, img1);
Iy = filter2(dGs'*Gs, img1);
It = img2 - img1;
% [Ix, Iy, It] = gradient(cat(3,img1,img2));
% rad = 0;

% Crop to valid region
xx = rad+1:w-rad;
yy = rad+1:h-rad;
Ix = Ix(yy,xx);
Iy = Iy(yy,xx);
It = It(yy,xx);
I1 = img1(yy,xx);
I2 = img2(yy,xx);

Txx = Ix.*Ix;
Txy = Ix.*Iy;
Txt = Ix.*It;
Tyy = Iy.*Iy;
Tyt = Iy.*It;

[u, v, res, err] = GaussSeidel(Txx, Txy, Txt, Tyy, Tyt, regular, iterations, Ix, Iy, It, I1, I2);
imgw = WarpImage(I2, u, v);
diff = (imgw - I1).^2;
error = sqrt(mean(diff(:))) % RMS

% pad u and v fields:
upad = zeros(h,w);
vpad = zeros(h,w);
upad(rad+1:h-rad,rad+1:w-rad) = u;
vpad(rad+1:h-rad,rad+1:w-rad) = v;
u = upad;
v = vpad;
