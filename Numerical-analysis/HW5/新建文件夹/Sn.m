function int=Sn(f,x)
N=size(x,2);
N=N-1;
h=x(2)-x(1);
x2=x(2:2:N); 
x3=x(3:2:N-1); 
int=h*(f(x(1))+f(x(N+1))+2*sum(f(x3))+4*sum(f(x2)))/3;
