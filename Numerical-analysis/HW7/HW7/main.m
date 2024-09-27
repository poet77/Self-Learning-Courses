f=@(x)(sin(x)/x);
g=@(x)((cos(x)-exp(x))/sin(x));
h=@(x)1/(x*exp(1/x));
t=@(x)(1/x);
op=@(x)((x*x)/(pi*pi));

a=h(1);
b=op(2);


D1=Romberg(f,1e-16,1,6);
D2=Romberg(g,-1,-1*1e-16,6)+Romberg(g,1e-16,1,6);
D3=Romberg(h,1e-16,1,6);
D4=Romberg(t,1,3,6);
D5=Romberg(op,0,0.5*pi,6);

digits(6);

disp(vpa(D1));
disp(vpa(D2));
disp(vpa(D3));