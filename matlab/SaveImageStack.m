function [ ] = SaveImageStack( stack, format )
% SaveImageStack Saves a set of images with names following the file
% format.

for i = 1:size(stack,3)
    imwrite(uint16(stack(:,:,i)), sprintf(format, i));
end;
