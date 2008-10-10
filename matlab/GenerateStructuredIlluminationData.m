function [] = GenerateStructuredIlluminationData(params)
% Generates data for structured illumination tests.  Saves images to a data
% directory.
%
% Expects a params structure with the following data (example).
%
% params.filePattern = '/home/beastwoo/Nano/data/test/struct/%s-%02.2fsnr%+02.2fdz-%01.3ftheta%+02.2fdx%+02.2fdy-%04d.tif';
% params.snr = 7.0;
% params.dz = 0;
% params.theta = 0.2 * pi;
% params.dx = sort(unique([-8:0.5:8 -1:0.1:1]));
% params.dy = 0;
% params.fgName = 'none';
% params.Imax = 200;
% params.featSize = 110;
% params.imgSize = 4 * [params.featSize params.featSize];
% params.samples = 1;

%%
% Create a single grid that will be transformed with all the test
% paramters.

origGrid = MakeGrid([7 7]);
gridCenter = round(size(origGrid)/2);
gridCorner = gridCenter - round(params.imgSize/2);

%%
% Load the foreground image.
switch (params.fgName)
    case 'oct01'
        fgImage = double(imread('/home/beastwoo/Nano/data/kier/20080909-octopus/oct01-80dx-0000.tif'));
    case 'oct02'
        fgImage = double(imread('/home/beastwoo/Nano/data/kier/20080909-octopus/oct02-80dx-0000.tif'));
    case 'splat'
        g = GaussianKernel1D(3.0, 0, 3);
        gg = g'*g;
        fgImage = RandomSplat(size(origGrid), 0.985, gg);
        fgImage = ScaleData(-fgImage, [30 240]);
    otherwise
        fgImage = params.Imax * ones(size(origGrid));
end
    
fgimgCenter = round(size(fgImage)/2);
fgimgCorner = fgimgCenter - round(params.imgSize/2);

%%
% Vary all test parameters.

for snr = params.snr(:)'
    % determine grid transmission from SNR, and convert to background and
    % foreground intensity levels
    alpha = 1 - snr/sqrt(params.Imax);
    gridSNR = ScaleData(origGrid, [alpha 1]);
    
    for dz = params.dz(:)'
        % make a defocus kernel and smooth grid
        g = GaussianKernel1D(4.0, 0, 4);    % TODO: model defocus
        gridDz = filter2(g, gridSNR);
        gridDz = filter2(g', gridDz);
        
        for theta = params.theta(:)'
            for dx = params.dx(:)'
                for dy = params.dy(:)'
                    % create transform, transform grid
                    trans = CreateTransform(gridCenter, [1 1], theta, [dy dx]);
                    imgGrid = TransformImage(gridDz, params.imgSize, gridCorner, trans);
                    
                    % create transform, transform foreground image
                    trans = CreateTransform(fgimgCenter, [1 1], 0, [0 0]);
                    imgFg = TransformImage(fgImage, params.imgSize, fgimgCorner, trans);
                    
                    % attenuate forground image by grid
                    img = imgFg .* imgGrid;
                    
                    for s = 1:params.samples
                        % construct a file name
                        filename = sprintf(params.filePattern, params.fgName, snr, dz, theta, dx, dy, s);
                        fprintf('%s\n', filename);
                        % save image with poisson noise
                        imwrite(uint8(poissrnd(img)), filename);
                    end     % samples
                end     % dy
            end     % dx
        end     % theta
    end     % dz
end     % snr

