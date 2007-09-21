function [ ] = SaveFigure( filename )
% SaveFigure(filename) - Saves an image with the contents of the current
% figure window.
% Extracts the contents from the current MATLAB figure as a frame, and
% saves to an image file.  Compression is set to none.  This is useful to
% create images from matlab figures that have been drawn and that don't
% include the annoying border that MATLAB uses if you save a figure from
% the figure file menu.
%
% IMPORTANT: The current figure must not be obscured by any other window.
% The MATLAB getframe function seems to do a screen capture from region
% occupied by the figure, so any windows that are overlayed will be
% included in this region.  Also, there seems to be a width limit for some
% figures beyond which the image is cropped.  Not sure what causes that.
%
% Input
%   filename    - The name of the file in which to save the figure data.
%   Should probably be a tif file, but other extensions might work.
%
% Output
%   
img = getframe;
imwrite(img.cdata, filename, 'Compression', 'none');
