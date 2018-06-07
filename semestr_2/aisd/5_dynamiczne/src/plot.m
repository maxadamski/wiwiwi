m = csvread('output/3d.csv')
n = m(1, 2:end)
b = m(2:end, 1)
t = m(2:end, 2:end)

h = figure(1)
surf(b, n, t)
xlabel('b', 'FontSize', 13)
ylabel('n', 'FontSize', 13)
zlabel('t', 'FontSize', 13)

H = 4; W = 4;
set(h, 'PaperUnits', 'inches')
set(h, 'PaperSize', [H, W])
set(h, 'PaperPosition', [0, 0, W, H])
input('wait...')
