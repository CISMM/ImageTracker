function [ ] = ShowVectorField( field, count, scale )
%SHOWVECTORFIELD Summary of this function goes here
%   Detailed explanation goes here

if (nargin < 3)
    scale = 1;
end;
if (nargin < 2)
    count = 1000;
end;

[h w d] = size(field);
stride = round(h*w/count);

dispimg(ones(h,w));

hold on;
for i = 0:stride:(h*w)-1
    y = floor(i/w);
    x = i - w*y + 1;
    y = h - y;
    
    plot(x,y,'b.');
    plot([x; x+scale*field(x,y,1)], [y, y+scale*field(x,y,2)], 'b-');
end;
hold off;