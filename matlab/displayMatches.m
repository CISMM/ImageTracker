function [ ] = DisplayMatches( img1, img2, matches )
%DISPLAYMATCHES Summary of this function goes here
%  Detailed explanation goes here

resp = '';
flip = 1;

display('Press enter to flip between images.  Press q to quit.');
colormap gray;
while (~strcmp(resp, 'q'))
    clf;
    
    if (flip)
        displayFeatures(img1, matches(:,1:2), 'r+');
    else
        displayFeatures(img2, matches(:,3:4), 'r+');
    end;
    
    flip = ~flip;
    resp = input('', 's');   
end;
    