function [ imgs ] = TransformImage( img, outSize, offset, center, angle, translate, steps )
% TransformImage Apply an affine transform to an input image in a number of steps.
%
% img        - the input image
% outSize    - the size of the output images
% offset     - the position of the bottom left corner of the output images
% center     - the center of rotation
% angle      - the angle through which to rotate
% translate  - the translation to apply
% steps      - the number of steps to take

% Parameters to increment at each step
dAngle = (1/steps) * angle;
dTran = (1/steps) * translate;

% Center of rotation matrices
centerMat = [1 0 -center(2); 0 1 -center(1); 0 0 1];
returnMat = [1 0 center(2); 0 1 center(1); 0 0 1];

for i = 1:steps
    display(sprintf('Step %d of %d', i, steps));
    % compute total angle and translation for this step
    iAngle = i * dAngle;
    iTran = i * dTran;
    
    % rotation and translation transform matrices
    rotMat = [cos(iAngle) -sin(iAngle) 0; sin(iAngle) cos(iAngle) 0; 0 0 1];
    tranMat = [1 0 iTran(2); 0 1 iTran(1); 0 0 1];
    
    % total transform matrix
    tran = tranMat * returnMat * rotMat * centerMat;
    
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
    
%     pos = zeros(outSize(1),outSize(2),3); % initialize sampling matrix
%     for y = 1:outSize(1)
%         for x = 1:outSize(2)
%             % find the index from the original image corresponding to the
%             warped [x, y, 1]
%             pos(y, x, :) = invTran * [x+offset(2) y+offset(1) 1]';
%         end;
%     end;

%     imgs(:,:,i) = interp2(img, squeeze(pos(:,:,1)), squeeze(pos(:,:,2)), 'linear');
end;
