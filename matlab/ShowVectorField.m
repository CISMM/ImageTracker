function [ ] = ShowVectorField( field, count, scale, background )
% ShowVectorField(field, count, scale, backgrnd) Depicts a vector field as
% a needle diagram.
% Displays a needle diagram for a vector field (e.g. a flow field).
% Inputs (default):
% field       - The vector field, as a h x w x 2 vector, where the 2 vector
% contains (x,y) vector information
% count       - The number of vectors to display; larger numbers create
% denser plots (1000)
% scale       - The scaling to apply to each vector; larger numbers create
% longer needles (1)
% backgnd     - A background image to display before drawing the needle
% points.

if (nargin < 4)
    background = [];
end;
if (nargin < 3)
    scale = 1;
end;
if (nargin < 2)
    count = 1000;
end;

% Determine sample stride based on field dimensions
[h w d] = size(field);
hw = h*w;
stride = round(hw/count);

% Displaying an image gets MATLAB in the imaging, not plotting, mood.
if (sum(background(:)) == 0)
    dispimg(ones(h,w));
else
    dispimg(background);
end;

% Create a grid of pixel locations
xx = 1:w;
yy = 1:h;
[xGrid, yGrid] = meshgrid(xx, yy);

% Determine sample indices, in absolute terms
ii = 1:stride:(hw);

hold on;
% Plot dots at the image grid locations
plot(xGrid(ii), yGrid(ii), 'b.');
for i = 1:length(ii)
    % Plot a needle at each grid location, based on the vector field
    plot([xGrid(ii(i)); xGrid(ii(i))+scale*field(ii(i))], [yGrid(ii(i)); yGrid(ii(i))+scale*field(hw+ii(i))], 'b-');
end;
hold off;
