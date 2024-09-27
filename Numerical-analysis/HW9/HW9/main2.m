test=@(t,y)((t-exp(-t))/(y+exp(y)));

t0=0;
y0=0;
z0=zeros(1,6);
error=zeros(1,6);
order=zeros(1,5);
for k=3:8
h=1/2^k;
X=linspace(0,1,2^k+1);
[t1,y1]=RungeKutta5(test,t0,y0,k);

for n=6:2^k+1

y1(n)=y1(n-1)+h*(1901*test(t1(n-1),y1(n-1))-2774*test(t1(n-2),y1(n-2))+2616*test(t1(n-3),y1(n-3))-1274*test(t1(n-4),y1(n-4))+251*test(t1(n-5),y1(n-5)))/720;
t1(n)=t1(n-1)+h;
end
z0(k-2)=y1(2^k+1);
error(k-2)=z0(k-2)+1;
end

for p=2:6
order(p-1)=log(error(p-1)/error(p))/log(2);
end