% corner detector
%
function points=HarrisAtScale(img,sInt,sDiff,count)

if (nargin < 4)
    count = 1000;
end

[h,w]=size(img);
tiles=round((w+h)/30); 

dG = GaussianKernel1D(sDiff, 1, 4);
% G = fspecial('gaussian', ceil(sDiff*5), sDiff);
% dGx = conv2(G, dG,  'same');
% dGy = conv2(G, dG', 'same');
Ix = conv2(img, dG,  'same');
Iy = conv2(img, dG', 'same');

Ixx=Ix.*Ix;
Ixy=Ix.*Iy;
Iyy=Iy.*Iy;

G=fspecial('gaussian',ceil(sInt*5),sInt);

GIxx=filter2(G,Ixx);
GIxy=filter2(G,Ixy);
GIyy=filter2(G,Iyy);

% Weighting function (Harris corner detector)
R=(GIxx.*GIyy-GIxy.*GIxy)-0.04*(GIxx+GIyy).^2;

ii=2:h-1;
jj=2:w-1;

% Find maximum in 4-neighborhoods
M(ii,jj)=(R(ii,jj)>R(ii+1,jj-1)).*(R(ii,jj)>R(ii+1,jj)).*(R(ii,jj)>R(ii+1,jj+1)).*(R(ii,jj)>R(ii,jj-1)).*(R(ii,jj)>R(ii,jj+1)).*(R(ii,jj)>R(ii-1,jj-1)).*(R(ii,jj)>R(ii-1,jj)).*(R(ii,jj)>R(ii-1,jj+1));
M(h,w)=0;

% Harris score for all maxima
RM=R.*M;

% Find indices of non-zero scores
[cy,cx,r]=find(RM);

% Sort by Harris score
[tmp,i]=sort(r);

countI = min(count-1, length(i)-1);
it=i(length(i):-1:length(i)-countI);

ncorners=length(it);

% clf;
% colormap('gray')
% imagesc(GIxx + GIyy); hold on;
% plot(cx(it),cy(it),'r+')

% Return points [Y,X]
points=[cy(it) cx(it)];