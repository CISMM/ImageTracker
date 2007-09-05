% C=NCCpatch(I1,I2);
%
% computes normalized cross correlation between two square or circular image patches
%
% I1,I2 image patches (should be square and have same size)
% type: square=0; circle=1;

function C=NCCpatch(I1,I2,type)

if (nargin<3)
    type=1; % type=1: ellipse; type=0: square
end

if size(I1)~=size(I2)
    disp('patchsize incorrect');
    C=0;
else
    
    % select region
    if (type~=1)
        I1=reshape(I1,prod(size(I1)),1);
        I2=reshape(I2,prod(size(I2)),1);
    else
        I1tmp=[];I2tmp=[];
        r=(size(I1,1)-1)/2;
        for i=-r:r,
            j=round(sqrt(r^2-i^2));
            rj=[r+1-j:r+1+j]; ri=r+1+i;
            I1tmp=[I1tmp; I1(rj,ri)];
            I2tmp=[I2tmp; I2(rj,ri)];
        end
        I1=I1tmp;
        I2=I2tmp;
    end
    
    %normalize
    % The norm below computes the variance of the original data.
    % Clever Marc Pollefeys.
    I1=double(I1); I2=double(I2);
    I1=I1-mean(I1); I1=I1/norm(I1);
    I2=I2-mean(I2); I2=I2/norm(I2);
    
    C=I1'*I2;
end
