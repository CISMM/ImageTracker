function grid = MakeGrid(repeat)
% MakeGrid simulates a binary grid image with the given number of
% repetitions in y and x.

barWidth = 26;
holeWidth = 84;

corner = ones(barWidth, barWidth);
bar = ones(barWidth, holeWidth);
hole = zeros(holeWidth, holeWidth);
pattern = [corner bar; bar' hole];
grid = repmat(pattern, repeat(1), repeat(2));
