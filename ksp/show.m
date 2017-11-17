ans_output;
ans_size = size(ans);
x_size = ans_size(2);
y_size = ans_size(1);
step = 0.002;
x = 0:step:pi;
y = 0:step:1;
[x,y] = meshgrid(x,y);
mesh(x,y,ans);