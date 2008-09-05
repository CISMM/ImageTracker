%% Phase Correlation for Structured Illumination Microscopy

%% 
clear;
close all;
format compact;
global debug;
debug = false;

%% FFT during translation
% The frequency magnitude of a signal is preserved under translation.  The
% phase encodes this information.
t = 1:1024;
patSize = 16;
[xx, yy] = meshgrid(t,t);
img = cos(2*pi*xx/patSize) + sin(2*pi*yy/patSize);
img = ScaleData(img, [0 255]);

corner = [256 256];
imgs = TransformSequence(img, [256 256], corner, corner + 128, [1 1], 0, [0 16], 5);

%%
figure(1); clf;
N = size(imgs,3);
clear mont;
for i = 1:N
    wimg = WindowImage(imgs(:,:,i), 'none');
    fimg = fftshift(nthroot(abs(fft2(wimg)), 8));
    mont(:,:,i) = ImageMontage(cat(3, wimg, ScaleData(fimg, [0 255])), 1, 10, 0);
    dispimg(mont(:,:,i));
    drawnow; pause(0.1);
end
figure(2); clf;
dispimg(ImageMontage(mont, N, 0, 0));
axis off;

%% FFT during rotation
% Rotation of a pattern changes the orientation of the FFT.
t = 1:1024;
patSize = 16;
[xx, yy] = meshgrid(t,t);
img = cos(2*pi*xx/patSize) + sin(2*pi*yy/patSize);
img = ScaleData(img, [0 255]);

corner = [256 256];
imgs = TransformSequence(img, [256 256], corner, corner + 128, [1 1], pi/2, [0 0], 10);

%%
figure(1); clf;
N = size(imgs,3);
clear mont;
for i = 1:N
    wimg = WindowImage(imgs(:,:,i), 'none');
    fimg = fftshift(nthroot(abs(fft2(wimg)), 8));
    mont(:,:,i) = ImageMontage(cat(3, wimg, ScaleData(fimg, [0 255])), 1, 10, 0);
    dispimg(mont(:,:,i));
    drawnow; pause(0.1);
end
figure(2); clf;
dispimg(ImageMontage(mont, min(N,5), 0, 0));
axis off;

%% Phase correlation
% Translation in the image domain corresponds to a phase shift in the
% frequency domain.  Phase correlation relies on broad-spectrum image
% information to find the translation between two images.

% load a sample image
% img = double(rgb2gray(imread('davidbowie.jpg')));
img = double(rgb2gray(imread('KierOctopus.png')));

% create an image sequence
h = 256;
imgs = TransformSequence(img, [h h], [400 20], [1 1], [1 1], 0, [45, -40], 2);
% add noise
imgs = imgs + 20*rand(size(imgs));

figure(1); clf;
dispimg(ImageMontage(imgs, 2, 10, 0));

%%

% phase correlate
[dt corrmap] = PhaseCorrelation(imgs(:,:,end), imgs(:,:,1));
figure(1); clf;
NiceSurf(corrmap); axis tight;

% interpret correlation map
fprintf('Computed translation (x,y): (%d, %d)\n', dt(1), dt(2));

%% Phase correlation with patterns
% Even a small amount of noise confounds phase correlation of a pattern
% that does not have broad spectrum support.
t = 1:1024;
patSize = 16;
[xx, yy] = meshgrid(t,t);
img = cos(2*pi*xx/patSize) + sin(2*pi*yy/patSize);
img = ScaleData(img, [0 255]);
img = imrotate(img, rad2deg(pi/10), 'bilinear', 'crop');
clf; dispimg(img); drawnow;

corner = [256 256];
imgs = TransformSequence(img, [256 256], corner, corner, [1 1], 0, [7, -5], 2);
% add a little noise
imgs = imgs + 0.5*randn(size(imgs));
figure(1); clf;
dispimg(ImageMontage(imgs, 2, 10, 0));

%%

% phase correlate
[dt corrmap] = PhaseCorrelation(imgs(:,:,end), imgs(:,:,1));
figure(1); clf;
NiceSurf(corrmap); axis tight;

fprintf('Computed translation (x,y): (%d, %d)\n', dt(1), dt(2));

%%
% Applying a band-pass filter during phase correlation to restrict the
% frequencies used in the computation improves matters, but the broad peaks
% in the correlation map point to uncertainty in the measurement.

% phase correlate
[dt corrmap] = PhaseCorrelation(imgs(:,:,end), imgs(:,:,1), patSize);
figure(1); clf;
NiceSurf(corrmap); axis tight;

fprintf('Computed translation (x,y): (%d, %d)\n', dt(1), dt(2));

%% 
% It is more effective to search for the specific frequency components for
% which phase correlation should be computed.
debug = true;
[disp, phaz, rot] = PhaseShiftDisplacement(imgs(:,:,1), imgs(:,:,end), patSize);
fprintf('Computed translation (x,y): (%0.4f, %0.4f)\n', disp);

%% Square patterns
% Checkerboard patterns have dominant frequency components oriented along the
% diagonals of the squares.
patSize = 32;
img = checkerboard(ceil(patSize/2), ceil(1024/patSize)) > 0.5;
img = ScaleData(img, [0 255]);
img = imrotate(img, 45, 'bilinear', 'crop');

corner = [256 256];
imgs = TransformSequence(img, [256 256], corner, corner, [1 1], 0, [2.3, -1.1], 2);
imgs = imgs + 10*randn(size(imgs));

figure(1);clf;
dispimg(ImageMontage(imgs, 2, 10, 0));

%%
% This frequency-specific phase correlation is not limited to integer
% pixel displacements.
debug = true;
[disp, phaz, rot] = PhaseShiftDisplacement(imgs(:,:,1), imgs(:,:,end), sqrt(2)/2*patSize);
fprintf('Computed translation (x,y): (%0.4f, %0.4f)\n', disp);

%% Simulated structured illumination
% Modulating a translating image with a simulated pattern provides an idea
% of how this tracking method will work in practice.

% create pattern
patSize = 45;
pattern = checkerboard(ceil(patSize/2), ceil(1024/patSize)) > 0.5;
pattern = imrotate(pattern, 45, 'bilinear', 'crop');
pattern = ScaleData(pattern, [0.5 1]);

% modulate with example image
img = double(rgb2gray(imread('davidbowie.jpg')));
img = ScaleData(img, [0 255]);
[h,w] = size(img);
imgPat = img .* pattern(256:256+h-1,256:256+w-1);

corner = [64 96];
imgs = TransformSequence(imgPat, [256 256], corner, corner, [1 1], 0, [-5.1 9.4], 2);
imgs = imgs + 10*randn(size(imgs));

figure(1);clf;
dispimg(ImageMontage(imgs, 2, 10, 0));

%% Find displacement of features
debug = true;
[disp, phaz, rot] = PhaseShiftDisplacement(imgs(:,:,1), imgs(:,:,end), sqrt(2)/2*patSize);
fprintf('Computed translation (x,y): (%0.4f, %0.4f)\n', disp);

%% Simulated stage motion
% An image of a TEM grid pattern on a number 0 cover slip is loaded and
% translation is simulated.
img = double(imread('~/Nano/data/grids/grid01-320z-dx/grid01-0000.tif'));
img = ScaleData(img, [0 255]);
imgs = TransformSequence(img, [380 380], [20 20], [1 1], [1 1], 0, [3.4 -8.3], 2);
imgs = imgs + 5*randn(size(imgs));

figure(1);clf;
dispimg(ImageMontage(imgs, 2, 10, 0));

%%
% An attempt to register these images using normal phase correlation.
[dt corrmap] = PhaseCorrelation(imgs(:,:,end), imgs(:,:,1));
figure(1); clf;
NiceSurf(corrmap); axis tight;

fprintf('Computed translation (x,y): (%d, %d)\n', dt(1), dt(2));

%% 
% Use frequency-specific phase correlation.
debug = true;
[disp, phaz, rot] = PhaseShiftDisplacement(imgs(:,:,1), imgs(:,:,end), 110/2);
fprintf('Computed translation (x,y): (%0.4f, %0.4f)\n', disp);

%% Real data
% The following is a sequence of the TEM pattern translated by the Mad City
% Lab stage.

imgs = LoadImages('~/Nano/data/grids/grid01-300z-dx/grid01-%04d.tif', 0, 59);
imgs = imgs(11:450, 11:450, :);
imgs = 255 * (imgs-min(imgs(:))) / (max(imgs(:))-min(imgs(:)));

figure(1); clf;
N = size(imgs,3);
montSet = round(linspace(1,N,5));
dispimg(ImageMontage(imgs(:,:,montSet), 5, 10, 0));

%%
% Attempt to register these images using band-limited phase correlation.
[dt corrmap] = PhaseCorrelation(imgs(:,:,end), imgs(:,:,1), 110);
figure(1); clf;
NiceSurf(corrmap); axis tight;

fprintf('Computed translation (x,y): (%d, %d)\n', dt(1), dt(2));

%% 
% Use frequency-specific phase correlation.
debug = true;
[disp, phaz, rot] = PhaseShiftDisplacement(imgs(:,:,end), imgs(:,:,1), 110);
fprintf('Computed translation (x,y): (%0.4f, %0.4f)\n', disp);

%%
% Track using phase correlation.
debug = false;
dispPx = PhaseShiftTrack(imgs, 109);

%%
% Apply physical units and a tracking marker.

% create a pattern to display
[h, w, N] = size(imgs);
mid = round([w h] / 2);
offset = [-50 0 50];
patX = mid(1) + offset;
patY = mid(2) + offset';
[patXX, patYY] = meshgrid(patX, patY);
patXX = patXX(:);
patYY = patYY(:);
patXX = patXX([1 7 9 3 1 4 6 3 2 8]);
patYY = patYY([1 7 9 3 1 4 6 3 2 8]);

figure(1); clf;
uppx = 0.228;
dispUm = dispPx * uppx;
for i = 1:N
    msg = sprintf('Frame %2d\ncenter: [%10.4f, %10.4f] px\ndisp: [%10.4f, %10.4f] um', i, dispPx(i,:), dispUm(i,:));
    dispimg(imgs(:,:,i), msg);
    ApplyImageUnits(gca, uppx, '\mum');
    hold on; plot(patXX + dispPx(i,1), patYY + dispPx(i,2), 'b-'); hold off;
    drawnow;
    pause(0.1);
end
%% 
% Mosaic
figure(1); clf;
mosaic = StitchImages(imgs, dispPx);
ApplyImageUnits(gca, uppx, '\mum');

%% Real Specimen
% The following image sequences are taken of octopus tentacle muscle,
% provided by Bill Kier.  A 1000 mesh grid pattern on a #0 cover slip was
% fixed to the slide over some muscle regions.
imgs = LoadImages('~/Nano/data/kier/20080821-octopus-01/oct-30umdx-%04d.tif', 0, 139);
[h, w, N] = size(imgs);
imgs = imgs(:, 120:120+h-1, :);
imgs = 255 * (imgs-min(imgs(:))) / (max(imgs(:))-min(imgs(:)));
[h, w, N] = size(imgs);

figure(1); clf;
montSet = round(linspace(1,N,5));
dispimg(ImageMontage(imgs(:,:,montSet), 5, 10, 0));
ShowMovie(imgs);

%%
% Track using phase correlation.
debug = false;
dispPx = PhaseShiftTrack(imgs, 110/2);

%%
% Apply physical units and a tracking marker.

% create a pattern to display
[h, w, N] = size(imgs);
mid = round([w h] / 2);
offset = [-50 0 50];
patX = mid(1) + offset;
patY = mid(2) + offset';
[patXX, patYY] = meshgrid(patX, patY);
patXX = patXX(:);
patYY = patYY(:);
patXX = patXX([1 7 9 3 1 4 6 3 2 8]);
patYY = patYY([1 7 9 3 1 4 6 3 2 8]);

figure(1); clf;
uppx = 0.228;
dispUm = dispPx * uppx;
for i = 1:N
    msg = sprintf('Frame %2d\ncenter: [%10.4f, %10.4f] px\ndisp: [%10.4f, %10.4f] um', i, dispPx(i,:), dispUm(i,:));
    dispimg(imgs(:,:,i), msg);
    ApplyImageUnits(gca, uppx, '\mum');
    hold on; plot(patXX + dispPx(i,1), patYY + dispPx(i,2), 'b-'); hold off;
    drawnow;
    pause(0.1);
end
%% 
% Mosaic.

% load full file sequence
% imgs = LoadImages('~/Nano/data/kier/20080821-octopus-01/oct-30umdx-%04d.tif', 0, 139);
% imgs = imgs(5:end-5,5:end-10,:);
figure(1); clf;
mosaic = StitchImages(imgs, dispPx);
dispimg(mosaic);
ApplyImageUnits(gca, uppx, '\mum');

%% Focus
% Defocusing acts as a low pass filter on an object.  A low pass filter
% attenuates the high frequency components of our grid pattern.  Finding
% how changing focus affects the magnitude of the frequency components from
% our pattern should provide a method of tracking in Z.
%
% Here, the stage is moved in Z, changing the focus of the pattern.
imgs = LoadImages('~/Nano/data/grids/grid01-300z+dz/grid01-%04d.tif', 0, 99);
imgs = imgs(11:450, 11:450, :);
imgs = 255 * (imgs-min(imgs(:))) / (max(imgs(:))-min(imgs(:)));

figure(1); clf;
N = size(imgs,3);
montSet = round(linspace(1,N,5));
dispimg(ImageMontage(imgs(:,:,montSet), 5, 10, 0));

%%
% We perform frequency-specific phase correlation, and retrieve the
% amplitude of the pattern fundamental and harmonics in all frames.
debug = false;
clear harmonics;
tic;
for i=1:N-1
    [disp, phaz, rot, harmonics(i,:)] = PhaseShiftDisplacement(imgs(:,:,i), imgs(:,:,i+1), 110);
end
elapsed = toc;
fprintf('Elapsed time: %6.2f\tFPS: %6.2f\n', elapsed, (size(imgs,3)/elapsed));

%%
% One possible relative focus metric is the cummulative ratios of harmonic
% amplitudes between consecutive images.
harmRat = log(abs(harmonics(:,4:6)))-log(abs(harmonics(:,1:3)));
plot(exp(cumsum(harmRat(:,1:3))));

title(sprintf('Relative focus metric:\ncomparing amplitudes'));
xlabel('Frame');
ylabel('Magnitude ratio');
legend({num2str((1:3)')});

%%
% One possible absolute focus metric is the fall-off between the
% fundamental and first harmonic in one frame.
fallOff = abs(harmonics(:,2))./abs(harmonics(:,1));
plot(fallOff);
title(sprintf('Absolute focus metric:\nharmonic fall-off'));
xlabel('Frame');
ylabel('1st harmonic / fundamental');