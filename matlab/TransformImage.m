function [ imgs, dx, dy ] = TransformImage( img, outSize, offset, center, scale, angle, translate, steps )
% TransformImage Apply an affine transform to an input image in a number of steps.
%
% img        - the input image
% outSize    - the size of the output images
% offset     - the position of the bottom left corner of the output images
% center     - the center of rotation/scaling (wrt the original image)
% scale      - the scaling to apply
% angle      - the angle through which to rotate
% translate  - the translation to apply
% steps      - the number of steps to take

% Parameters to increment at each step
dAngle = (1/steps) * angle
dTran = (1/steps) * translate
dScale = (1/steps) * (scale-1)

% Center of rotation matrices
centerMat = [1 0 -center(2); 0 1 -center(1); 0 0 1];
returnMat = [1 0 center(2); 0 1 center(1); 0 0 1];

for i = 1:steps
    display(sprintf('Step %d of %d', i, steps));
    % compute total angle and translation for this step
    iAngle = i * dAngle;
    iTran = i * dTran;
    iScale = i * dScale;
    
    % rotation and translation transform matrices
    rotMat = [cos(iAngle) -sin(iAngle) 0; sin(iAngle) cos(iAngle) 0; 0 0 1];
    scaleMat = [1+iScale(2) 0 0; 0 1+iScale(1) 0; 0 0 1];
    tranMat = [1 0 iTran(2); 0 1 iTran(1); 0 0 1];
    
    % total transform matrix
    tran = tranMat * returnMat * rotMat * scaleMat * centerMat;
    
    % inverse transform--this is for tear-free image warping
    invTran = inv(tran);
    
    xx=(1:outSize(2)) + offset(2);
    yy=(1:outSize(1)) + offset(1);
    [xx,yy] = meshgrid(xx,yy);
    hh=ones(outSize(1),outSize(2));
    pos = [xx(:)'; yy(:)'; hh(:)'];
    pos = invTran * pos;
    
    posX = reshape(pos(1,:), outSize(1), outSize(2));
    posY = reshape(pos(2,:), outSize(1), outSize(2));
    imgs(:,:,i) = interp2(img, posX, posY, 'linear', 0);
    dx(:,:,i) = xx - posX;
    dy(:,:,i) = yy - posY;
end;
