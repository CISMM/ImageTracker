% corner detector
%
function c=harris2(I,target)

if (nargin==1)
    target = 1000;
end

% I=double(rgb2gray(I));
[h,w]=size(I);
tiles=round((w+h)/30); 

Ix=filter2([1 2 1; 0 0 0; -1 -2 -1],I);
Iy=filter2([1 0 -1; 2 0 -2; 1 0 -1],I);

Ixx=Ix.*Ix;
Ixy=Ix.*Iy;
Iyy=Iy.*Iy;

G=fspecial('gaussian',5,0.7);

GIxx=filter2(G,Ixx);
GIxy=filter2(G,Ixy);
GIyy=filter2(G,Iyy);

R=(GIxx.*GIyy-GIxy.*GIxy)-0.04*(GIxx+GIyy).^2;

ii=2:h-1;
jj=2:w-1;

M(ii,jj)=(R(ii,jj)>R(ii+1,jj-1)).*(R(ii,jj)>R(ii+1,jj)).*(R(ii,jj)>R(ii+1,jj+1)).*(R(ii,jj)>R(ii,jj-1)).*(R(ii,jj)>R(ii,jj+1)).*(R(ii,jj)>R(ii-1,jj-1)).*(R(ii,jj)>R(ii-1,jj)).*(R(ii,jj)>R(ii-1,jj+1));
M(h,w)=0;

RM=R.*M;

[cx,cy,r]=find(RM);

[tmp,i]=sort(r);
cutoff = min(target-1, length(i));
it=i(length(i):-1:length(i)-cutoff);

ncorners=length(it)

% clf;
% colormap('gray')
% imagesc(I); hold on;
% plot(cy(it),cx(it),'x')

c=[cx(it) cy(it)];