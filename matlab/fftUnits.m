function [freq] = fftUnits(N, rate)
% FFTUnits creates an array containing the frequencies for the FFT (fourier
% transform) of a data set.  This requires the number of samples in the
% data set and the sampling rate of the data.  The output array can be used
% as the x-axis for a frequency plot.  For example, assume the array 'data'
% holds some 1D data set sampled at 120 Hz.
%
%   N = length(data);
%   fftData = fft(data);
%   plot(fftUnits(N, 120), fftshift(abs(fftData)));
%   xlabel('Frequency (Hz)');
%   ylabel('Magnitude');
%
% Note that the array returned is aligned with the fftshifted data returned
% from MATLAB's fft function.  Here, the units returned from fftUnits are
% in Hz.
%
% Input:
%   N -     the number of samples in the data array
%   rate -  the sampling rate of the data (optional)
%
% Output:
%   freq -  frequencies in the FFT of the data in proper units

if (nargin < 2)
    rate = 1;
end

ii = 1:N;
center = floor(N/2) + 1;
freq = (ii-center)/N * rate;
