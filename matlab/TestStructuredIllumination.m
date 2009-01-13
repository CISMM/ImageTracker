%% Test Structured Illumination
% Load a number of data sets and run structured illumination tracking.
% This is an attempt to catalogue when the tracking seems to work and when
% it fails.

%%
close all;
clear all;
format compact;
global debug;
debug = false;
doBandPass = false;

%% Simulation
% Create a simulation of a grid moving with known displacements, perform
% frequency-based tracking on it, and evaluate the performance.
%
% First, create a large grid image.
g = GaussianKernel1D(3.0, 0, 4);
gg = g'*g;

rotTrue = -pi/7;

grid = MakeGrid([20 20]);
grid = ScaleData(grid, [0 255]);
grid = imrotate(grid, rad2deg(rotTrue), 'bicubic', 'crop');
grid = filter2(gg, grid, 'same');
gridMid = round(size(grid)/2);
imgSize = [440 440];
imgMid = round(imgSize / 2);

%%
% Simulate motion.
[imgs, tran] = TransformSequence(grid, imgSize, gridMid-imgMid, gridMid, [1 1], 0, [70 -70], 10);

% add noise
% imgs = imgs + 10*poissrnd(imgs);
imgs = imgs + 32*randn(size(imgs));

%%
% Track grid.
RunStructuredIllumination;

% evaluate error
dxTrue = squeeze(tran(1:2,3,:))';
rmsError = sqrt(sum((dxTrue - dxPhase).^2, 2))
dispTrue = sqrt(sum(dxTrue.^2, 2));
rmsErrorPerPixel = rmsError ./ dispTrue

rots = rot + [-pi : pi/2 : pi]

%% Simulate octopus and grid
octopus = double(imread('octopus-mosaic.tif'));
octopus = ScaleData(octopus, [0 255]);
[h, w] = size(octopus);
gridcrop = grid(200:200+h-1, 500:500+w-1);
gridcrop = ScaleData(gridcrop, [0.45 1]);
octgrid = octopus .* gridcrop;
dispimg(octgrid);

imgSize = [440 440];
[imgs, tran] = TransformSequence(octgrid, imgSize, [1 1], [1 1], [1 1], 0, [0 -140], 30);
imgs = imgs + 2 * randn(size(imgs));
ShowMovie(imgs);

%%
% Track
RunStructuredIllumination;

% evaluate error
dxTrue = squeeze(tran(1:2,3,:))';
rmsError = sqrt(sum((dxTrue - dxPhase).^2, 2))
dispTrue = sqrt(sum(dxTrue.^2, 2));
rmsErrorPerPixel = rmsError ./ dispTrue

rots = rot + [-pi : pi/4 : pi]

%% Pattern
% The following data sets have only the structured grid present.
%
% Here the grid is close to in focus.  The tracking seems good.
imgs = ReadPulnix('/home/beastwoo/Nano/data/grids/grid01-40x-1fps-1upsx-300z.raw');
imgs = double(imgs(11:450, 111:550, :));
RunStructuredIllumination;

%%
% Here the grid is out of focus.
imgs = ReadPulnix('/home/beastwoo/Nano/data/grids/grid01-40x-1fps-1upsy-260z.raw');
imgs = double(imgs(11:450, 111:550, :));
RunStructuredIllumination;

%%
% Here the grid is at an angle; the stage moves in x.
imgs = ReadPulnix('~/Nano/data/kier/20080916-octopus/grid01-40x-000um+040dx.raw');
imgs = double(imgs(11:450, 111:550, 10:4:end));
RunStructuredIllumination;

%%
% Grid at an angle, moves in X.
imgs = ReadPulnix('~/Nano/data/kier/20081010-grids/grid01-40x+000z+040dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
% This is a 12nm gold plated grid, moved by hand.
imgs = ReadPulnix('~/Nano/data/kier/20081010-grids/grid01-2cr12au-40x-000z-hand.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
% This is the 12nm grid, out of focus by +10 um.
imgs = ReadPulnix('~/Nano/data/kier/20081010-grids/grid02-2cr12au-40x+010z-hand.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:end));
RunStructuredIllumination;

%%
% This is a 20nm grid, which has high SNR.
imgs = ReadPulnix('~/Nano/data/kier/20081210-grids/grid20-su02-40x-000z-40dx.raw', [51 200]);
imgs = double(imgs(11:450, 111:550, :));
RunStructuredIllumination;

%%
imgs = ReadPulnix('~/Nano/data/kier/20081215-grids/grid20-01-40x-000z-40dx.raw');
imgs = double(imgs(11:450, 111:550, :));
RunStructuredIllumination;

%% Octopus
% The following data sets have only octopus muscle present.
%
% The stage pans in x.  The errors in stitching here seem no worse than
% some sequences with the patterned substrate.  Drift becomes apparent as
% the stage returns to its initial position.  There is a slight jump in y
% when the stage starts to move.
imgs = ReadPulnix('~/Nano/data/kier/20080909-octopus/oct01-40x-000z-80dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
% The stage pans in y.  There is definitely some horizontal drift in the
% tracking for this sequence.
imgs = ReadPulnix('~/Nano/data/kier/20080909-octopus/oct01-40x-000z+80dy.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
% The muscle region imaged here has less fibrous structure than found in
% other regions.  This is toward the center of one of the sections.
% 
% This image sequence has a jump in y early on, which the tracking picks
% up.  The tracking is OK, but not great--seams are apparent during image
% stitching.
imgs = ReadPulnix('~/Nano/data/kier/20080909-octopus/oct02-40x-000z-80dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
% The stage pans in y.  The pesky MCL puts another jump in x in the
% beginning of this video, which is not handled well.  The remainder of the
% video seems to stitch well, however.
imgs = ReadPulnix('~/Nano/data/kier/20080909-octopus/oct02-40x-000z+80dy.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%% Frog
% The following videos are of tungara midbrain sections.

imgs = ReadPulnix('~/Nano/data/kier/20081010-grids/frog01-40x+000z+040dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%% Octopus and Pattern
% The following videos are of regions that include both the muscle and the
% patterned substrate.
%
% If one pays attention to the apparent tracking drift perpendicular to the
% dominant direction of motion for each of these movies, one notes that
% this drift is completely consistent with the image sensor being rotated
% slightly with respect to the stage.  In fact, the camera attached to the
% micrsocope with which these images were taken rotates readily.
%
% The stage moves in x from the edge of a muscle section towards the
% middle.  Tracking and stitching seem reliable.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid01-40x-000um-30umdx.raw');
imgs = double(imgs(11:450, 11:450, 1:2:end));
RunStructuredIllumination;

%%
% In the following two examples, the stage pans in y along the edge of a
% muscle section.  Each sequence is a crop of the *same* original video.
% We display the band pass filtered video as well, to get an idea of the
% information content present that we are using to track.
%
% In the first, the crop is done such that the edge of the section is not
% visible in most of the frame.  This seems to track and stitch well.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid01-40x-000um+30umdy.raw');
imgs = double(imgs(11:450, 201:640, 1:2:end));
doBandPass = true;
RunStructuredIllumination;
doBandPass = false;

%%
% In this crop, the edge of the muscle is near the middle of the image
% frame throughout the sequence.  Here the tracking fails.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid01-40x-000um+30umdy.raw');
imgs = double(imgs(11:450, 11:450, 1:2:end));
doBandPass = true;
RunStructuredIllumination;
doBandPass = false;

%%
% This image sequence is taken towards the center of a section where there
% is also the patterned substrate.
%
% The stage pans in x.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid02-40x-000um+30umdx.raw');
imgs = double(imgs(11:450, 11:450, 1:2:end));
RunStructuredIllumination;

%%
% The stage pans in y.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid02-40x-000um-30umdy.raw');
imgs = double(imgs(11:450, 11:450, 1:2:end));
RunStructuredIllumination;

%%
% Here we focus through the specimen and onto the patterned substrate
% below.  Clearly stitching in this case makes no sense.  But, we do take a
% look at the band pass images....note that they do not change because the
% pattern frequency does not get extinguished by the defocusing.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080821-octopus/octgrid02-40x-000um+40umdz.raw');
imgs = double(imgs(11:450, 11:450, 1:2:end));
doBandPass = true;
RunStructuredIllumination;
doBandPass = false;

%%
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-octopus/octgrid03+40x-000um+040dx.raw');
imgs = double(imgs(11:450, 11:450, 1:4:end));
RunStructuredIllumination;

%%
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-octopus/octgrid04+40x-000um+040dx.raw');
imgs = double(imgs(11:450, 11:450, 1:4:end));
imgs = ScaleData(imgs, [0 255]);
RunStructuredIllumination;

%%
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-octopus/octgrid04+40x-000um+040dz.raw');
imgs = double(imgs(11:450, 11:450, end:-4:1));
imgs = ScaleData(imgs, [0 255]);
RunStructuredIllumination;

%%
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-octopus/octgrid05+40x-000um+040dx.raw');
imgs = double(imgs(11:450, 11:450, 1:4:end));
RunStructuredIllumination;

%% Tungara
% The following image sequences have only tungara midbrain tissue present.
%
% The stage pans in x.
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-tungara/frog01+40x-000um+040dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%

imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-tungara/frog01+40x-000um+040dy.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;

%%
imgs = ReadPulnix('/home/beastwoo/Nano/data/kier/20080916-tungara/frog02+40x-000um+040dx.raw', 200);
imgs = double(imgs(11:450, 111:550, 1:2:end));
RunStructuredIllumination;
