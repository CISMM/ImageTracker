function [tform] = CreateTransform(center, scale, rotate, translate)
% CreateTransform creates a transform matrix for a 2D transformation from a
% specified set of paramters.  The transformation applies a scaling,
% rotation, and translation (in that order).  There is also specified a
% center of rotation and scaling to use.  All parameters are specified as
% 2-element arrays (2x1 or 1x2) with pixel measurements except for the
% scalar rotation.
%
% Input:
%   center -    the center about which to scale and rotate
%   scale -     the scaling to apply
%   rotation -  the rotation to apply, in radians
%   translate - the translation to apply
% Output:
%   tform -     the 3x3 transform matrix that performs the specified
%   operations

centerMat = [1 0 -center(2); 0 1 -center(1); 0 0 1];
returnMat = [1 0 center(2); 0 1 center(1); 0 0 1];

rotMat = [cos(rotate) -sin(rotate) 0; sin(rotate) cos(rotate) 0; 0 0 1];
scaleMat = [scale(2) 0 0; 0 scale(1) 0; 0 0 1];
tranMat = [1 0 translate(2); 0 1 translate(1); 0 0 1];

tform = tranMat * returnMat * rotMat * scaleMat * centerMat;
