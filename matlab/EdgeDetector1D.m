dataIn = load('..\..\data\Jay\csvChromatinData');
dataIn = dataIn(:,[1,5]);
dataIn = dataIn(1:600000,:);
dataFilter = resizeGauss1D(dataIn,5);
figure;
plot(dataFilter(:,1), dataFilter(:,2));
figure;
plot(dataFilter(:,2));

dSpace = createGaussScaleSpace(dataFilter(:,2), 1, 1, 30, 0.5);
figure;
imagesc(dSpace(1:end-30,:));
% surf(dSpace(1:end-30,:));

maxima = findLocalMaxima(dSpace);
figure;
imagesc(maxima);
colormap gray;

ddSpace = createGaussScaleSpace(dataFilter(:,2), 2, 1, 30, 1.0);
crossings = findZeroCrossings(ddSpace);
figure;
imagesc(crossings);
colormap gray;