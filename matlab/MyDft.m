function dft = MyDft(img)
% My version of a discrete Fourier transform, to see if I know what I'm
% doing.

[h w] = size(img);
if (h ~= w)
    warning('Image must be square: %d ~= %d\n', h, w);
    return;
end

freq = 0:h-1;
x = 0:h-1;

M = cos(2*pi/h * (freq' * x)) - i*sin(2*pi/h * (freq' * x)); % y-direction fft

% We want the non-conjugate transpose of M!!!
dft = M * img;
dft = dft * transpose(M);
