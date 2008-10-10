function [imgOut, radiusOfConfusion, lens] = SimulateFocus(imgIn, lens, dFocus)
% SimulateFocus simulates the effect of viewing an image with an ideal lens
% at a given distance from optimal focus.
%
% Input:
%   imgIn -     the image to defocus
%   lens -      the parameters of the lens as follows:
%       .mag    the magnification of the lens
%       .NA     the numerical aperature
%       .n      the index of refraction of the medium
%       .workdist  the working distance, in meters
%       .cover  the thickness of the coverslip, in meters
%       .mppx   the physical size of a sensor element, in meters
%   dFocus -    the distance from optimal focus
%
% Output:
%   imgOut -    the defocused image
%   radiusOfConfusion - the radius of confusion in meters
%   lens -      the lens parameters including computed values:
%       .focusOpt   the optimal distance between the lens and specimen
%       .f          the focal length of the lens
%       .radius     the radius of the lens front aperture

% the distance from the lens to the specimen at optimal focus
lens.focusOpt = lens.workdist + lens.cover;

% the focal length of the lens
lens.f = lens.mag * lens.focusOpt/(1 + lens.mag);

% radius of the lens
lens.radius = lens.focusOpt * tan(asin(lens.NA/lens.n));

% the distance from the lens to the specimen
specDist = lens.focusOpt + dFocus;

% the distance between the optimal focus plane and the image of the
% specimen
% dFocusPlane = (lens.f/(imgDist+lens.f)) * ...
%     (lens.f/(lens.focusOpt + lens.f)) * dFocus;
dFocusLens = abs((lens.f * lens.focusOpt) / (lens.focusOpt - lens.f) - ...
    (lens.f * specDist) / (specDist - lens.f));

radiusOfConfusion = lens.radius * ...
    (lens.focusOpt/specDist * ...
    (specDist - lens.f)/(lens.focusOpt - lens.f) ...
    - 1);

% radius of confusion (radius of disk cast by specimen point onto image
% plane)
% radiusOfConfusion = abs(dFocusPlane * tan(asin(lens.NA/lens.n)) / lens.mag);

% create a filter image containing a disk of the proper size
radOfConfPix = radiusOfConfusion / lens.mppx;
filterSize = ceil(radOfConfPix) * 2 + 1;
[xx, yy] = meshgrid(1:filterSize, 1:filterSize);
mask = sqrt((xx-radOfConfPix-1).^2 + (yy-radOfConfPix-1).^2) <= radOfConfPix;
filter = mask;
% normalize filter to unit intensity
filter = filter ./ (sum(filter(:)));

% filter image
imgOut = filter2(filter, imgIn, 'same');
