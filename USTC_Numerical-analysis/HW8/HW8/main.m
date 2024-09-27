test1=@(t,y)(5*y+cos(t)-5*sin(t));
test2=@(t,y)(-5*y+cos(t)+5*sin(t));
test3=@(t,y)(-10*y+cos(t)+10*sin(t));
tspan=[0 5];
y0=0;

[t1,y1]=Runge(test1,tspan,y0);
[t2,y2]=Runge(test2,tspan,y0);
[t3,y3]=Runge(test3,tspan,y0);
error1=zeros(501,1);
error2=zeros(501,1);
error3=zeros(501,1);

for i=1:501
    error1(i)=abs(y1(i)-sin(t1(i)));
    error2(i)=abs(y2(i)-sin(t2(i)));
    error3(i)=abs(y3(i)-sin(t3(i)));
    t4(i)=t1(i);
    y4(i)=sin(t1(i));
end
figure(1);
plot(t1,y1,'r',t4,y4,'k');
figure(2);
plot(t2,y2,'r',t4,y4,'b');
figure(3);
plot(t3,y3,'r',t4,y4,'k');

