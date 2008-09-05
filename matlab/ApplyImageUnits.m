function [] = ApplyImageUnits(ax, unitsPerPixel, label)

if (nargin < 3)
    label = [];
end

unitX = num2str(unitsPerPixel*str2num(get(gca, 'XTickLabel')));
unitY = num2str(unitsPerPixel*str2num(get(gca, 'YTickLabel')));
set(gca, 'XTickLabel', unitX);
set(gca, 'YTickLabel', unitY);

if (~isempty(label))
    xlabel(label);
    ylabel(label);
end