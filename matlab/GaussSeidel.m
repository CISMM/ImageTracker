function [ u, v, res, err ] = GaussSeidel( Txx, Txy, Txt, Tyy, Tyt, regular, iterations, Ix, Iy, It, I1, I2)
% GAUSSSEIDEL Summary of this function goes here
%   Detailed explanation goes here

[h, w] = size(Txx);

% Pre-compute the denominator
denom = (regular^2 + Txx + Tyy);
denom = 1./denom;

% Laplace estimation kernel
laplace = 1/12 * [1 2 1; 2 -12 2; 1 2 1];
laplacebar = laplace;
laplacebar(2,2) = 0;

% Derivative estimation kernel (for smoothness)
dx = 1/2 * [-1 0 1];
dy = dx';

% initial conditions
u = zeros(h, w);
v = zeros(h, w);
lastU = u;
lastV = v;

xx = 2:w-1;
yy = 2:h-1;

% pre-initialize variables that grow within loop
resc = zeros(1,iterations);
ress = zeros(1,iterations);
res = zeros(1,iterations);
err = zeros(1,iterations);

r = 12;
masky = r+1:h-r;
maskx = r+1:w-r;

for i = 1:iterations
    ubar = filter2(laplacebar, u);
    vbar = filter2(laplacebar, v);
    
    u = ubar - denom .* (Txx.*ubar + Txy.*vbar + Txt);
    v = vbar - denom .* (Txy.*ubar + Tyy.*vbar + Tyt);
    
    % Von Neumann boundary conditions; copy neighboring velocity values to
    % edge pixels
    u(1,xx) = u(2,xx);
    u(h,xx) = u(h-1,xx);
    u(yy,1) = u(yy,2);
    u(yy,w) = u(yy,w-1);
    u(1,1) = 0.5 * (u(1,2) + u(2,1));
    u(h,1) = 0.5 * (u(h,2) + u(h-1,1));
    u(1,w) = 0.5 * (u(2,w) + u(1,w-1));
    u(h,w) = 0.5 * (u(h,w-1) + u(h-1,w));
    
    v(1,xx) = v(2,xx);
    v(h,xx) = v(h-1,xx);
    v(yy,1) = v(yy,2);
    v(yy,w) = v(yy,w-1);
    v(1,1) = 0.5 * (v(1,2) + v(2,1));
    v(h,1) = 0.5 * (v(h,2) + v(h-1,1));
    v(1,w) = 0.5 * (v(2,w) + v(1,w-1));
    v(h,w) = 0.5 * (v(h,w-1) + v(h-1,w));
        
    % Calculate residual
    % optical flow constraint
    Ec = (Ix.*u + Iy.*v + It).^2;
    Ec = Ec(masky, maskx);
    
    % smoothness constraint
    Es = (filter2(dx,u).^2 + filter2(dy,v).^2 + filter2(dx,v).^2 + filter2(dy,v).^2).^2;
    Es = Es(masky, maskx);
    
    resc(i) = mean(Ec(:));
    ress(i) = regular^2 * mean(Es(:));
    res(i) = resc(i) + ress(i);

%     % Determine how much the answer has changed
%     du = mask.*abs(u - lastU); lastU = u;
%     dv = mask.*abs(v - lastV); lastV = v;
%     dui(i) = sum(du(:));
%     dvi(i) = sum(dv(:));
     
    % Determine error
    imgw = WarpImage(I2, u, v);
    diff = (imgw - I1).^2;
    diff = diff(masky, maskx);
    err(i) = sqrt(mean(diff(:))); % RMSE
    
    % Show progress
%     str = sprintf('u, v, Ec, Es, e, i = %d', i);
%     uprog = u(masky, maskx)/max(u(:));
%     vprog = v(masky, maskx)/max(v(:));
%     dispimg(ImageMontage(cat(3,uprog,vprog,Ec/max(Ec(:)),Es/max(Es(:)),diff/max(diff(:))), 2, 5, 0), str, 'hot'); 
%     drawnow;
end;
