function [ u, v, imwarp, res, err ] = MultiResOpticalFlow(img1, img2, levels, flowFcn)
% MultiResOpticalFlow(img1, img2, levels flowFcn) computes the optical flow
% from a pair of images using a multiresolution approach. The flow
% function, which computes the optical flow at one resolution level, is
% provided as a function handle.
%
% Input:
%   img1 -      the reference image for the flow computation
%   img2 -      the moving image for the flow computation
%   levels -    the number of resolution levels to use
%   flowFcn -   the function handle that computes flow between two images
% Output:
%   u -         the x-component of computed flow
%   v -         the y-component of computed flow
%   imwarp -    img2 warped with the computed flow field
%   res -       the value of the total optical flow residual during all
%   stages of the computation
%   err -       the value of the total image error during all stages of the
%   computation

if (nargin < 3)
    levels = 3;
end
if (nargin < 4)
    flowFcn = @(im1, im2) CLGOpticalFlow(im1, im2, 1.0, 4.0, 100, 1.9, 200);
end

[h, w] = size(img1);

if (levels > 1) % need to do a coarser scale first
    I1sm = imresize(img1, 0.5, 'bilinear');
    I2sm = imresize(img2, 0.5, 'bilinear');
    [usm, vsm, wrpsm, ressm, errsm] = MultiResOpticalFlow( I1sm, I2sm, levels-1, flowFcn);
    
    % warp img2 with upscaled velocity field.
    uUp = 2*imresize(usm, [h,w], 'bilinear');
    vUp = 2*imresize(vsm, [h,w], 'bilinear');
    I2 = WarpImage(img2, uUp, vUp);
else
    I2 = img2;
    uUp = zeros(h,w);
    vUp = zeros(h,w);
    ressm = [];
    errsm = [];
end;

% do flow computation at this scale
[u, v, imwarp, res, err] = flowFcn(img1, I2);

% concatenate residuals and errors
res = [ressm res];
err = [errsm err];

% display
% figure(2);
% subplot(2,1,1); plot(res); title('residual');
% subplot(2,1,2); plot(err); title('error');

% add this scale's flow to the total
u = u + uUp;
v = v + vUp;
imwarp = WarpImage(img2, u, v);

% display
% figure(1);
% ShowMovie(cat(3,img2,img1,imwarp), 1);
