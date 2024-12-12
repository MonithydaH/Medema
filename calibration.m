
clc;clear;close all;
% x=[0.5 1 1.5 2 2.5 3 3.5 4 4.5 5 5.5 6 6.5 7 7.5 8 8.5 9 9.5 10 10.5 11 11.5 12 12.5 13 13.5 14 14.5 15];
% y=[0 0 0.07 0.25 0.32 0.51 0.63 0.84 0.88 1.01 1.13 1.21 1.35 1.55 1.68 1.79 1.98 2.1 2.21 2.34 2.45 2.64 2.79 2.89 2.95 3.16 3.26 3.27 3.3 3.3];
% figure
% plot(x,y)

x = [0 0.5 1 1.5 2 2.5 3 3.5 4 4.5 5 5.5 6 6.5 7 7.5 8 8.5 9 9.5 10 10.5 11 11.5 12 12.5 13 13.5 14 14.5 15]; %data point using caliper
y = [0 0 0 0.07 0.25 0.32 0.51 0.63 0.84 0.88 1.01 1.13 1.21 1.35 1.55 1.68 1.79 1.98 2.1 2.21 2.34 2.45 2.64 2.79 2.89 2.95 3.16 3.26 3.27 3.3 3.3];%voltage 0-3.3v

% Scatter plot
figure;
scatter(x, y, 'filled');
hold on;

% Best-fit line
coefficients = polyfit(x, y, 1); %find coefficient of slope and intercept
y_fit = polyval(coefficients, x);% evaluate the line of equation at each x
slope=coefficients(1);
intercept=coefficients(2);
disp(['Calibration Equation: y = ', num2str(slope), 'x ', num2str(intercept)]);
% Plot best-fit line
plot(x, y_fit,'LineWidth',1.5);
hold off;

% Add labels and title
xlabel('Distance (mm)');
ylabel('Voltage(v)');
title('Data Plot with Best-Fit Line');
legend('Data Points', 'Best-Fit Line');
