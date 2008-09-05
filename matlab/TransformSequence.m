function [imgs, dx, dy] = TransformSequence(img, outSize, offset, center, scale, angle, translate, steps)
% TransformSequence creates a video of an affine transform on an input image.
%
% Input:
%   img        - the input image
%   outSize    - the size of the output images
%   offset     - the position of the upper left corner of the output images
%   center     - the center of rotation/scaling (wrt the original image)
%   scale      - the scaling to apply
%   angle      - the angle through which to rotate
%   translate  - the translation to apply
%   steps      - the number of steps to take

% preallocate output
imgs = zeros(outSize(1), outSize(2), steps);
dx = zeros(outSize(1), outSize(2), steps);
dy = zeros(outSize(1), outSize(2), steps);

% Parameters to increment at each step
dAngle = (1/(steps-1)) * angle;
dTran = (1/(steps-1)) * translate;
dScale = (1/(steps-1)) * (scale-1);

for i = 1:steps
    % display(sprintf('Step %d of %d', i, steps));
    % compute total angle and translation for this step
    iAngle = (i-1) * dAngle;
    iTran = (i-1) * dTran;
    iScale = 1 + (i-1) * dScale;
    
    tran = CreateTransform(center, iScale, iAngle, iTran);
    
    [imgs(:,:,i), dx(:,:,i), dy(:,:,i)] = TransformImage(img, outSize, offset, tran);
end;