function [dispPx] = PhaseShiftTrack(imgs, featSize)

[h, w, N] = size(imgs);

mid = round([w h]/2);
rad = 50;

% create a target grid
mask = [-rad rad];
[gridX, gridY] = meshgrid(mask + mid(1), mask + mid(2));
gridX = gridX(:);
gridY = gridY(:);
gridX = gridX([1 2 4 3 1]);
gridY = gridY([1 2 4 3 1]);
lineHX = [1; w];
lineVX = [50:50:w; 50:50:w];
lineHY = [50:50:h; 50:50:h];
lineVY = [1; h];

% create a tracker window
[midX, midY] = meshgrid(mid(1)-rad:mid(1)+rad, mid(2)-rad:mid(2)+rad);

dispPx = zeros(N, 2);

for i = 2:N
    [disp, shift, rot] = PhaseShiftDisplacement(imgs(:,:,i-1), imgs(:,:,i), featSize);
    fprintf('Rotation: %0.3f\n', rot);
    dispPx(i,:) = dispPx(i-1,:) + disp;
    
    % display tracker and tracker-eye view
    figure(1); clf;
    subplot(1,2,1);
    dispimg(imgs(:,:,i));
    hold on; 
    plot(gridX + dispPx(i,1), gridY + dispPx(i,2), 'r-');
    plot(lineHX, lineHY, 'b-');
    plot(lineVX, lineVY, 'b-');
    hold off;
    msg = sprintf('frame: %d\ndisp: %0.3f %0.3f', i, dispPx(i,:));
    title(msg);

    subplot(1,2,2);
    midWind = interp2(imgs(:,:,i), midX + dispPx(i,1), midY + dispPx(i,2), 'cubic', 0);
    dispimg(midWind);
    drawnow;
end