function [ output_args ] = displayTracks( img1, img2, matches )
%DISPLAYTRACKS Summary of this function goes here
%  Detailed explanation goes here

figure;
colormap gray;
resp = '';
flip = 1;

xline = [matches(:,2)';matches(:,4)'];
yline = [matches(:,1)';matches(:,3)'];

display('Press enter to flip between images.  Press q to quit.');
while (~strcmp(resp, 'q'))
    clf;
    
    if (flip)
        imagesc(img1);
        hold on;
        plot(matches(:,2), matches(:,1), 'r.');
        for i=1:size(xline,2)
            line(xline(:,i), yline(:,i));
        end;
        hold off;
    else
        imagesc(img2);
        hold on;
        plot(matches(:,4), matches(:,3), 'r.');
        hold off;
    end;
    
    flip = ~flip;
    resp = input('', 's');   
end;



