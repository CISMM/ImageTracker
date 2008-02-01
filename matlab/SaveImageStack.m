function [ ] = SaveImageStack( stack, format, varargin )
% SaveImageStack Saves a set of images with names following the file
% format.
% Might we suggest for TIFF images, add the following args:
% 'Resolution', [25.4 25.4]

dims = ndims(stack);

if (dims == 3) % stack of 2-d images
    for i = 1:size(stack,3)
        imwrite(uint16(stack(:,:,i)), sprintf(format, i-1), varargin{:});
    end;
elseif (dims == 4) % stack of vector fields
    for i = 1:size(stack,4)
        WriteMHAVector(sprintf(format, i-1), squeeze(stack(:,:,:,i)));
    end;
end;