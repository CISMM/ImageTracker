function [vectors] = ReadMHAVector(filename)
% Reads a meta-image vector file.
% Returns the data set as a matrix with dimensions [height, width,
% elements], where height and width are the size of the vector field and
% elements is the number of elements in each vector. The elements in the
% vector are returned in the order provided in the image file.

% Things to read:
%    NDims = 3
%    DimSize = 256 256
%    ElementNumberOfChannels = 2
%    ElementType = MET_UCHAR
%    ElementDataFile = LOCAL
%
% Things to skip:
%    ElementSpacing = 1 1
%    Offset = 0 0
%    ElementByteOrderMSB = False

% open the file
[file, msg] = fopen(filename, 'r');
if (file == -1)
    display(sprintf('Failed to open file: %s\n\t%s\n', filename, msg));
    return;
end;

% [fname,perm,form,enc] = fopen(file)

% set defaults
ndims = 0;
dimsize = 0;
elems = 0;
type = 'float32';
datafile = '';

% parse the header
% display(sprintf('Reading header...'));
name = 'junk';
while ((~strcmp(name, 'ElementDataFile')))
    % read next line
    line = fgetl(file);
    idx = strfind(line, ' = ');
    name = line(1:idx-1);
    value = line(idx+3:end);
    % display(sprintf('Line:\t%s', line));

    % parse the current parameter name
    if (strcmp(name, 'NDims'))
        ndims = sscanf(value, '%d');
    elseif (strcmp(name, 'DimSize'))
        dimsize = sscanf(value, '%d');
    elseif (strcmp(name, 'ElementNumberOfChannels'))
        elems = sscanf(value, '%d');
    elseif (strcmp(name, 'ElementType'))
        % todo
    elseif (strcmp(name, 'ElementDataFile'))
        datafile = sscanf(value, '%s');
    end;
end;
% display(sprintf('Done with header.\n'));

% makesure we have data
if (ndims == 0 || sum(dimsize) == 0 || elems == 0 || strcmp(datafile, ''))
    display(sprintf('Unable to read all header information.  I got:'));
    display(sprintf('\tNDims = %d\n\tDimSize = %d\n\tElements = %d\n\tType = %s\n\tDataFile = %s', ndims, dimsize, elems, type, datafile));
end;

% Read the data
vectors = zeros(dimsize(2), dimsize(1), elems);
for y = 1:dimsize(2)
    row = fread(file, [elems, dimsize(1)], type);
    vectors(y,:,:) = row';
end;

fclose(file);