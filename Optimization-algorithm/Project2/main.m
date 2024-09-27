Q = [4, -1; -1, 2];
c = [1; 1];

% 目标函数
f = @(x) 0.5 * x' * Q * x - c' * x;

% 梯度函数
grad_f = @(x) Q * x - c;

% Hessian矩阵函数
hess_f = @(x) Q;

% 等式约束函数
A = [1, 1];
b = 2;
eq_constraint = @(x) A * x - b;
grad_eq_constraint = @(x) A;

x0 = [0; 0];  % 初始点
epsilon = 1e-6;  % 停止准则
max_iter = 100;  % 最大迭代次数

% 牛顿法求解
x_star = newton_method(f, grad_f, hess_f, eq_constraint, grad_eq_constraint, x0, epsilon, max_iter);

disp("最优解 x* = ");
disp(x_star);