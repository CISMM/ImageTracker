function [hsurf] = NiceSurf(height, cmap)

hsurf = surf(height);

if (nargin > 1)
    set(hsurf, 'CData', cmap, 'FaceColor', 'texturemap');
else
    hsurf = surf(height);
    colormap summer;
    set(hsurf, 'FaceColor', 'interp');
end

axis ij;
set(hsurf, 'LineStyle', 'none');
material([0.1 0.5 0.5 10]);
lighting phong;
camlight right;
