function [ Z, C, cp, cq ] = FourierIntegrate( p, q )
% FourierIntegrate(dx, dy) Computes the surface, F, from the two partial
% derivatives, dF/dx and dF/dy.  Uses the Fourier transform of the
% derivatives, fp and fq, respectively.

[Nx, Ny] = size(p);

% Compute Fourier transforms.
% cp = myDFT(p);
% cq = myDFT(q);
cp = fft2(p);
cq = fft2(q);

% Find the frequencies for the fourier analysis.
ii = 0:Nx-1;
jj = 0:Ny-1;
fundFreqX = 2*pi() / Nx;
fundFreqY = 2*pi() / Ny;

disp(sprintf('Fundamental frequency: [%f,%f]', fundFreqX, fundFreqY));

wx = ii * fundFreqX;
wy = jj * fundFreqY;

C = zeros(Nx, Ny);

PrintImageInfo(real(cp), 'Real dft of dIdx');
PrintImageInfo(imag(cp), 'Imaginary dft of dIdx');
PrintImageInfo(real(cq), 'Real dft of dIdy');
PrintImageInfo(imag(cq), 'Imaginary dft of dIdy');

% Combine complex images according to Frankot & Chellappa.
for (j = 1:Ny)
    for (i = 1:Nx)
        % ax = complex(0, -wx(i));
        % ay = complex(0, -wy(i));
        ax = conj(complex(0, sin(wx(i))));
        ay = conj(complex(0, sin(wy(j))));
        denom = imag(ax)^2 + imag(ay)^2;

        if (denom == 0) % avoid a divide by zero for DC term (w = (0,0))
            C(i,j) = complex(0);
        else
            C(i,j) = (ax * cq(i,j) + ay * cp(i,j)) / denom;
        end;
    end;
end;

PrintImageInfo(real(C), 'Real coefficients');
PrintImageInfo(imag(C), 'Imaginary coefficients');

% Z = myDFT(C, 1);
Z = ifft2(C);
