function [dispPx, rot] = PhaseShiftTrack(imgs, featSize)

[h, w, N] = size(imgs);

mid = round([w h]/2);
rad = featSize/2;

% create a target grid
mask = [-rad rad];
[gridX, gridY] = meshgrid(mask + mid(1), mask + mid(2));
gridX = gridX(:);
gridY = gridY(:);
gridX = gridX([1 2 4 3 1]);
gridY = gridY([1 2 4 3 1]);
lineHX = [1; w];
lineVX = [rad:rad:w; rad:rad:w];
lineHY = [rad:rad:h; rad:rad:h];
lineVY = [1; h];

% create a tracker window
[midX, midY] = meshgrid(mid(1)-rad:mid(1)+rad, mid(2)-rad:mid(2)+rad);

% create an axis for the pattern
xaxis = [0 0; 3*rad 0];
yaxis = [0 0; 0 3*rad];

% find orientation
[disp, thetaFD] = FrequencyDisplacement(imgs(:,:,1), imgs(:,:,2), featSize);
% [disp, thetaPSD] = PhaseShiftDisplacement(imgs(:,:,1), imgs(:,:,2), featSize);

% initialize displacement
dispPx = zeros(N, 2);

for i = 2:N
    img1 = imgs(:,:,i-1);
    img2 = imgs(:,:,i);
    
    [disp, rot] = FrequencyDisplacement(img1, img2, featSize, thetaFD);
%     [disp, rot] = PhaseShiftDisplacement(img1, img2, featSize, thetaPSD);
    rot = rot(1);
    dispPx(i,:) = dispPx(i-1,:) + disp;
    
    % rotate axis according to pattern orientation
    rotM = [cos(rot) sin(rot); -sin(rot) cos(rot)];
    rotXaxis = xaxis * rotM + mid(1);
    rotYaxis = yaxis * rotM + mid(2);
    
    % display tracker and tracker-eye view
    figure(3); clf;
    subplot(1,2,1);
    dispimg(img2);
    hold on; 
    % grid
    plot(lineHX, lineHY, 'b-');
    plot(lineVX, lineVY, 'b-');
    % track box
    plot(gridX + dispPx(i,1), gridY + dispPx(i,2), 'r-');
    % orientation axis
    plot(rotXaxis(:,1) + dispPx(i,1), rotXaxis(:,2) + dispPx(i,2), 'r-');
    plot(rotYaxis(:,1) + dispPx(i,1), rotYaxis(:,2) + dispPx(i,2), 'g-');
    hold off;
    msg = sprintf('frame: %d\ndx: %0.3f %0.3f\ndisp: %0.3f %0.3f\nrot: %0.3f', i, dispPx(i,:) - dispPx(i-1,:), dispPx(i,:), rot);
    title(msg);

    subplot(1,2,2);
    midWind = interp2(img2, midX + dispPx(i,1), midY + dispPx(i,2), 'cubic', 0);
    dispimg(midWind);
    title('tracker view');
    drawnow;
%     saveas(gca, sprintf('imgs/track-%04d.tif', i));
end