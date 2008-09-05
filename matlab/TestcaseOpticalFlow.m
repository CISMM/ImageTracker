%%
% This script generates evaluation elements from tests of optical flow
% computation.  It expects a structure test to exist before being called.

%%
% Compute optical flow.
result = RunOpticalFlowTest(test);

%%
% Plot psnr, a measure of how well the computed flow matches the known
% ground truth.
figure(1);
plot(test.vary.data(2:end), [result.psnr result.psnrIdeal result.psnrWarp]);
title('Flow peak SNR');
xlabel(sprintf('%s (%s)', test.vary.label, test.vary.units));
ylabel('PSNR (dB)');
legend({'psnr du','psnr dv', 'psnr ideal', 'psnr warp'});
fprintf('%d frames in %f seconds, %f s/f\n', N-1, result.elapsedTime, (result.elapsedTime/(N-1)));

%%
% Show one of the computed flow fields.
figure(2); clf;
ShowVectorField(cat(3, result.du(:,:,1), result.dv(:,:,1)), 1000, 20, test.imgs(:,:,1));
colormap gray;
