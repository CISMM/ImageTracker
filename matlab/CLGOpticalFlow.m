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
[Gs] = GaussianKernel1D(sigmaDeriv, 0, 3);
dGs = -GaussianKernel1D(sigmaDeriv, 1, 3);
[Gp, rad] = GaussianKernel1D(sigmaAperture, 0, 3);

% pad the images to avoid badness at edges
% rad = 4*rad;
I1 = padarray(img1, [rad rad], 'both', 'replicate');
I2 = padarray(img2, [rad rad], 'both', 'replicate');
% fhann = @(x) 1-cos(pi*x);
% I1 = WindowImage(img1, fhann, rad, 'replicate');
% I2 = WindowImage(img2, fhann, rad, 'replicate');


% Compute the image derivatives
Ix = filter2(Gs'*dGs, I1);
Iy = filter2(dGs'*Gs, I1);
It = I2 - I1;

Txx = Ix.*Ix;
Tyy = Iy.*Iy;
Txy = Ix.*Iy;
Txt = Ix.*It;
Tyt = Iy.*It;

% Smooth the image structure tensor (aperture integration) and scale it
% according to the regularization
Gpp = Gp'*Gp;
Txx = filter2(Gpp, Txx); % / regular;
Tyy = filter2(Gpp, Tyy); % / regular;
Txy = filter2(Gpp, Txy); % / regular;
Txt = filter2(Gpp, Txt); % / regular;
Tyt = filter2(Gpp, Tyt); % / regular;

dispimg(ImageMontage(cat(3, Txx, Txy, Txt, Tyy, Txy, Tyt), 3, 5, 0));
drawnow;

[u, v, res, err] = GaussSeidel(Txx, Txy, Txt, Tyy, Tyt, regular, iterations, Ix, Iy, It, I1, I2);

imgw = WarpImage(I2, u, v);
% diff = (imgw - I1).^2;
% error = sqrt(mean(diff(:))) % RMSE

% Crop to valid region
xx = rad+1:w+rad;
yy = rad+1:h+rad;
u = u(yy,xx);
v = v(yy,xx);

