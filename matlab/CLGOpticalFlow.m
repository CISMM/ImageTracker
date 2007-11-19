function [ u, v, img2warp, err ] = CLGOpticalFlow( img1, img2, sigmaDeriv, sigmaAperture, regular, relax, iterations )
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
Gs = GaussianKernel1D(sigmaDeriv, 0, 3);
dGs = GaussianKernel1D(sigmaDeriv, 1, 3);
Gp = GaussianKernel1D(sigmaAperture, 0, 3);

% Create the image structure tensor
Ix = filter2(Gs'*dGs, img1);
Iy = filter2(dGs'*Gs, img1);
It = img1 - img2;

Txx = Ix.*Ix;
Tyy = Iy.*Iy;
Txy = Ix.*Iy;
Txt = Ix.*It;
Tyt = Iy.*It;

% Smooth the image structure tensor (aperture integration) and scale it
% according to the regularization
Txx = filter2(Gp'*Gp, Txx) / regular;
Tyy = filter2(Gp'*Gp, Tyy) / regular;
Txy = filter2(Gp'*Gp, Txy) / regular;
Txt = filter2(Gp'*Gp, Txt) / regular;
Tyt = filter2(Gp'*Gp, Tyt) / regular;

T = cat(3, Txx, Tyy, Txy, Txy, Txt, Tyt);
dispimg(ImageMontage(T, 2, 5, 0)); pause(1);

u = zeros(h, w);
v = zeros(h, w);
xx = 2:w-1;
yy = 2:h-1;
xint = 20:w-20;
yint = 20:h-20;

[xxg, yyg] = meshgrid(1:w,1:h);
meanval = mean(img1(:));

lastU = u;
lastV = v;

for i = 1:iterations
    for y = 2:h-1
        for x = 2:w-1
            sumU = u(y-1,x) + u(y+1,x) + u(y,x-1) + u(y,x+1);
            sumV = v(y-1,x) + v(y+1,x) + v(y,x-1) + v(y,x+1);
            u(y,x) = (1-relax)*u(y,x) + relax * (sumU - (Txy(y,x).*v(y,x) + Txt(y,x)))/(4 + Txx(y,x));
            v(y,x) = (1-relax)*v(y,x) + relax * (sumV - (Txy(y,x).*u(y,x) + Tyt(y,x)))/(4 + Tyy(y,x));
        end;
%         sumU = u(y-1,xx) + u(y+1,xx) + u(y,xx-1) + u(y,xx+1);
%         sumV = v(y-1,xx) + v(y+1,xx) + v(y,xx-1) + v(y,xx+1);
%         u(y,xx) = (1-relax)*u(y,xx) + relax * (sumU - (Txy(y,xx).*v(y,xx) + Txt(y,xx)))/(4 + Txx(y,xx));
%         v(y,xx) = (1-relax)*v(y,xx) + relax * (sumV - (Txy(y,xx).*u(y,xx) + Tyt(y,xx)))/(4 + Tyy(y,xx));
    end;
    
    % Von Neumann boundary conditions: set the perimeter values to the same
    % as the computed boundary values (derivative = 0)
    u(1,xx) = u(2,xx);      v(1,xx) = v(2,xx);
    u(h,xx) = u(h-1,xx);    v(h,xx) = v(h-1,xx);
    u(yy,1) = u(yy,2);      v(yy,1) = v(yy,2);
    u(yy,w) = u(yy,w-1);    v(yy,w) = v(yy,w-1);
    
    display(sprintf('Step: %4d\t=> u:\t%f\t[%f\t%f]', i, mean(u(:)), min(u(:)), max(u(:))));
    display(sprintf('Step: %4d\t=> v:\t%f\t[%f\t%f]', i, mean(v(:)), min(v(:)), max(v(:))));
    
    % Compute difference
    diffU = sum(abs(u(:) - lastU(:)));
    diffV = sum(abs(v(:) - lastV(:)));
    display(sprintf('Step: %4d\t=> diff:\t[%f\t%f]', i, diffU, diffV));
    lastU = u;
    lastV = v;
    
    % Compute error; use motion information to warp I2 onto I1
    img2warp = interp2(img2, xxg+u, yyg+v, 'linear', meanval);
    diff = (img1-img2warp).^2;
    diffsm = diff(yint, xint);
    dispimg(diffsm); pause(0.01);
    err(i) = sum(diffsm(:));
    display(sprintf('Step: %4d\t=> e:\t%f', i, err(end)));
    % if (err(end) > err(max(end-1,1))) break; end;
    if (sum(diffU+diffV) < 0.000005 && err(end) > err(max(end-1,1))) break; end;
end;

% flow(:,:,1) = u;
% flow(:,:,2) = v;
