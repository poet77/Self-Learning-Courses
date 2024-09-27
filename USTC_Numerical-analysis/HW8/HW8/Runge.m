function [t,y,n]=Runge(f,tspan,y0)
h=0.01;
t0=tspan(1);
tn=tspan(2);

n=floor((tn-t0)/h);
t(1)=t0;
y(:,1)=y0;
for i=1:n
t(i+1)=t(i)+h;
k1=f(t(i),y(:,i));
k2=f(t(i)+h/2,y(:,i)+h*k1/2);
k3=f(t(i)+h/2,y(:,i)+h*k2/2);
k4=f(t(i)+h,y(:,i)+h*k3);
y(:,i+1)=y(:,i)+h*(k1+2*k2+2*k3+k4)/6;

end
