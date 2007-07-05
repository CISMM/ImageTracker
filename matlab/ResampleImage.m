function [ imgOut ] = ResampleImage( imgIn, wind, func )
% ResampleImage(imgIn, wind, func) Resamples an input image over a given
% window with a given distortion function.
% 

% Determine bounds of the window
yy = wind(1,1):wind(1,2);
xx = wind(2,1):wind(2,2);

% Create a sampling grid from the window
[xGrid, yGrid] = meshgrid(xx,yy);

[h, w] = size(xGrid);

% Use the function handle to distort the positions in the sampling grid
yxSamp = func(yGrid, xGrid);

% Extract the results; func returns the results as a [y x]' matrix; so the
% top of the matrix are all the y positions and the bottom of the matrix
% are all the x positions.
ySamp = yxSamp(1:h,:);
xSamp = yxSamp(h+1:end,:);

% Resample the input image
imgOut = interp2(imgIn, xSamp, ySamp);
