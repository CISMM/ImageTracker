function [ u, v, imgw, res, err ] = CLGOpticalFlow( img1, img2, sigmaDeriv, sigmaAperture, regular, relax, iterations )
% CLGOpticalFlow Combined local global optical flow implementation of
% Bruhn, Weickert, and Schnorr.

% Set up default parameters
if (nargin < 7)
    iterations = 200;
end;
if (nargin < 6)
    relax = 1.9;
end;
if (nargin < 5)
    regular = 500;
end;
if (nargin < 4)
    sigmaAperture = 4.0;
end;
if (nargin < 3)
    sigmaDeriv = 1.0;
end;

[h, w] = size(img1);

% Create smoothing and differentiation kernels
[Gs, rad] = GaussianKernel1D(sigmaDeriv, 0, 3);
dGs = -GaussianKernel1D(sigmaDeriv, 1, 3);
[Gp] = GaussianKernel1D(sigmaAperture, 0, 3);

% Compute the image derivatives
% Compute the image derivatives
% dG = 1/2 * [-1 0 1]; % rad = 1;
% Ix = filter2(dG, img1);
% Iy = filter2(dG', img1);
Ix = filter2(Gs'*dGs, img1);
Iy = filter2(dGs'*Gs, img1);
It = img2 - img1;
% [Ix, Iy, It] = gradient(cat(3,img1,img2));
% Ix = Ix(:,:,1); Iy = Iy(:,:,1); It = It(:,:,1);
% rad = 0;

Txx = Ix.*Ix;
Tyy = Iy.*Iy;
Txy = Ix.*Iy;
Txt = Ix.*It;
Tyt = Iy.*It;

% Smooth the image structure tensor (aperture integration) and scale it
% according to the regularization
Txx = filter2(Gp'*Gp, Txx); % / regular;
Tyy = filter2(Gp'*Gp, Tyy); % / regular;
Txy = filter2(Gp'*Gp, Txy); % / regular;
Txt = filter2(Gp'*Gp, Txt); % / regular;
Tyt = filter2(Gp'*Gp, Tyt); % / regular;

% Crop to valid region
xx = rad+1:w-rad;
yy = rad+1:h-rad;
Ix = Ix(yy,xx);
Iy = Iy(yy,xx);
It = It(yy,xx);
I1 = img1(yy,xx);
I2 = img2(yy,xx);
Txx = Txx(yy,xx);
Tyy = Tyy(yy,xx);
Txy = Txy(yy,xx);
Txt = Txt(yy,xx);
Tyt = Tyt(yy,xx);

[u, v, res, err] = GaussSeidel(Txx, Txy, Txt, Tyy, Tyt, regular, iterations, Ix, Iy, It, I1, I2);
imgw = WarpImage(I2, u, v);
diff = (imgw - I1).^2;
error = sqrt(mean(diff(:))) % RMS

% pad u and v fields:
upad = mean(u(:)) * ones(h,w);
vpad = mean(v(:)) * ones(h,w);
upad(rad+1:h-rad,rad+1:w-rad) = u;
vpad(rad+1:h-rad,rad+1:w-rad) = v;
u = upad;
v = vpad;
