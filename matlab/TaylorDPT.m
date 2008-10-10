function dptUV = TaylorDPT(data, wavelengths, theta)
% TaylorDPT computes the Russell Taylor discrete phase transform for the
% given data set and collection of frequencies.  The input image is rotated
% to correct for the orientation, theta.  The rotated image is correlated
% with a set of sine and cosine functions of the given wavelengths, with
% consideration for handling partial wavelengths.  The correlations are
% used to compute the phase and magnitude of the oriented frequency
% components.
%
% Input
%   data -      the image data to correlate
%   freq -      the frequencies to generate sine waves to correlate against
%   theta -     the orientation to compute along

% make sure wavelengths is a column vector with integer elements
wavelengths = round(wavelengths(:));
freqs = 1./wavelengths;
waveN = length(wavelengths);

% create a mask the same size as the input data
mask = ones(size(data));

% rotate the image data and the mask in the same manner
% imrotate applies the proper inverse rotation (positive angles rotate
% counter-clockwise, but in a left hand coordinate system). Rotating the
% image means we are doing computations in the oriented UV space.
rotData = imrotate(data, rad2deg(theta), 'bilinear');
rotMask = imrotate(mask, rad2deg(theta), 'bilinear');
[h, w] = size(rotData);

% project data to the U and V axes
dataU = sum(rotData,1);     % 1 x w
dataV = sum(rotData,2)';    % 1 x h
maskU = sum(rotMask,1);
maskV = sum(rotMask,2)';

% replicate data for all wavelengths
dataU = repmat(dataU, waveN, 1);    % waveN x w
dataV = repmat(dataV, waveN, 1);    % waveN x h

% build a set of sine and cosine waves for each wavelength; the waves span
% the oriented image data.
uu = freqs * (0:w-1);     % waveN x w, anchor at uu=0 on edge
vv = freqs * (0:h-1);     % waveN x h
tranU = cos(2*pi*uu) - i * sin(2*pi*uu);
tranV = cos(2*pi*vv) - i * sin(2*pi*vv);

% point-wise multiply sines by data--correlation
corrU = dataU .* tranU;
corrV = dataV .* tranV;

dptUV = zeros(waveN,2);

% handle partial wavelength contributions--do each wavelength separately
for wIdx = 1:waveN
    % the set of bins
    bins = 1:wavelengths(wIdx);
    
    % the bin indices for each image index
    binIdU = mod((1:w)-1, wavelengths(wIdx)) + 1;
    binIdV = mod((1:h)-1, wavelengths(wIdx)) + 1;
    
    % the correlation bins in both directions
    corrBinU = zeros(wavelengths(wIdx), 1);
    corrBinV = zeros(wavelengths(wIdx), 1);
    
    % compute weight for each bin
    for bin = bins
        % count of elements in bin
        weightU = sum(maskU(binIdU == bin));
        weightV = sum(maskV(binIdV == bin));
        % correlation normalized by element count
        corrBinU(bin) = sum(corrU(wIdx, binIdU == bin)) / weightU;
        corrBinV(bin) = sum(corrV(wIdx, binIdV == bin)) / weightV;
    end
    
    % sum total correlation for this wavelength
    dptUV(wIdx,:) = [sum(corrBinU) sum(corrBinV)];
end

global debug;
if (debug)
    xpos = 1:wavelengths(1):w;
    
    figure(2); clf;
    subplot(3,2,1);
    dispimg(data);
    subplot(3,2,2);
    dispimg(mask);
    subplot(3,2,3);
    dispimg(rotData);
    subplot(3,2,4);
    dispimg(rotMask);
    subplot(3,2,5);
    plot(dataU);
    axis tight;
    DrawVLines(gca, xpos);
    subplot(3,2,6);
    plot(maskU);
    axis tight;
    DrawVLines(gca, xpos);
    
    figure(3); clf;
    subplot(2,2,1);
    plot(real(corrU(1,:)));
    axis tight;
    DrawVLines(gca, xpos);
    subplot(2,2,2);
    plot(imag(corrU(1,:)));
    axis tight;
    DrawVLines(gca, xpos);
    subplot(2,2,3);
    plot(real(corrBinU));
    axis tight;
    subplot(2,2,4);
    plot(imag(corrBinU));
    axis tight;
end

function DrawVLines(ax, xpos)
ypos = get(ax, 'YLim')';
ypos = repmat(ypos, 1, length(xpos));
xpos = repmat(xpos, 2, 1);
line(xpos, ypos, 'Color', 'r', 'LineStyle', ':');
