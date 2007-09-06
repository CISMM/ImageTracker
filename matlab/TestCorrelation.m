
img = double(rgb2gray(imread('davidbowie.jpg')));
img = img(50:305,50:305);

% build an image consisting of different frequencies in each direction
% t = (0:255)/256;
% x = sin(2*pi*7*t) + 0.7*cos(2*pi*20*t); % frequencies: 7, 20
% y = sin(2*pi*5*t) - 0.7*cos(2*pi*13*t); % frequencies: 5, 18
% img = y'*x;

% create a template
c = [164,207];
r = 3;
tem = img(c(1)-r:c(1)+r,c(2)-r:c(2)+r);

% add noise
% img = img + 0.10*(max(img(:))-min(img(:)))*randn(size(img));
% brighten
img = 1.05 * (img - min(img(:))) + 50;

figure; dispimg(img);
figure; dispimg(tem);

% compute correlation, find maximum
nccmap = CorrMap(img,tem);
[i,j] = find(nccmap >= max(nccmap(:))-0.005)
figure; dispimg(nccmap);
hold on;
plot(j,i,'r+');
hold off;