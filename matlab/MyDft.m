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

% Create a wierd weighting term that counts how many periods are included
% in each fourier component
% wt = [0 1:(h-1)/2 (h/2):-1:1];
% wtM = repmat(wt, h, 1) + repmat(wt', 1, h);
% wtM(1,1) = 1;
% wtM = 1 ./ wtM;
% wtM(1,1) = 0; % set DC term to zero

% We want the non-conjugate transpose of M!!!
dft = (M * img * transpose(M)); % .* wtM;
