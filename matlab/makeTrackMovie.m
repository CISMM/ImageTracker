function [ mov ] = makeTrackMovie( images, matches )
%DISPLAYMULTITRACKS Summary of this function goes here
%  Detailed explanation goes here
colormap gray;

mov = [];
for i=1:size(matches,1)
    img = images(i).img;
    match = matches(i).matches;
    if (length(match) == 0)
        match = [0 0 0 0];
    end;
    xline = [match(:,2)';match(:,4)'];
    yline = [match(:,1)';match(:,3)'];
    
    imagesc(img); axis image;
    hold on;
    plot(match(:,2), match(:,1), 'g.');
    for l=1:size(xline,2)
        line(xline(:,l), yline(:,l));
    end;
    hold off;
    mov = cat(2, mov, getframe);
    
    img = images(i+1).img;
    imagesc(img); axis image;
    hold on;
    plot(match(:,4), match(:,3), 'g.');
    hold on;
    mov = cat(2, mov, getframe);
end;
