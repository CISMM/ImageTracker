function [ avg, stdev ] = RotationDeviation( width, degrees )
% RotationDeviation computes the average magnitude and deviation expected
% from an image that has been rotated the given number of degrees about the
% center.

% Find the center of the image (use a 1-based index)
if mod(width, 2) == 0
    w = width + 1;
else
    w = width
end;

center = [w/2 w/2];
mags(1) = 0;

for y = 1:width
    ycomp = abs(y-center(2))^2;
    for x = 1:width
        distance = sqrt(abs(x-center(1))^2 + ycomp);
        rotMag = 2 * sin(deg2rad(0.5 * degrees)) * distance;
        mags(end+1,1) = rotMag;
    end;
end;

mags = mags(2:end);
avg = mean(mags);
stdev = std(mags);