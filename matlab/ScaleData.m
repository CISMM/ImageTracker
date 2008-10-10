function [data] = ScaleData(data, range)
% ScaleData rescales data to lie within the bounds of range.  If range is a
% single number, rescales to [-range range].
%
% Input
%   data -	the data to rescale
%   range -	the range over which to rescale the data
%
% Output
%   data -	the scaled data

if (nargin < 2)
    range = [-1 1];
end

if (numel(range) < 2)
    range = [-range range];
end

% scale data between zero and 1
data = (data - min(data(:)))/(max(data(:)) - min(data(:)));

% scale data between range(1) and range(2)
data = (range(2)-range(1)) * data + range(1);
