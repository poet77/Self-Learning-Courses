test=@(t,y)exp(y*t)+cos(y-t);

t0=1;
y0=3;
[t1,y1]=RungeKutta(test,t0,y0);

plot(t1,y1,'r');

 disp(["解的范围是:" [1,t1(5)]]);
 z0=input("输入一个范围内的数");

 b=3+(z0-1)*(y1(4)-3)/(t1(5)-1);

 disp(["对应的函数值为:" b] );
