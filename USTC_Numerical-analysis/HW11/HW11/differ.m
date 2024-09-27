function [y]=differ(a,b,n,alpha,beta,u,v,w,f)

h=(b-a)/(n+1);
y(1)=alpha;
y(n+2)=beta;

for i=1:n
    al(i)=-1-0.5*h*w(a+(i+1)*h);
    dl(i)=2+h*h*v(a+i*h);
    cl(i)=-1+0.5*h*w(a+i*h);
    bl(i)=-h*h*u(a+i*h);
end

A=zeros(n,n);
A(1,1)=dl(1);
A(1,2)=cl(1);
A(n,n-1)=al(n-1);
A(n,n)=dl(n);
for k=2:n-1
    A(k,k)=dl(k);
    A(k,k+1)=cl(k);
    A(k,k-1)=al(k-1);
end

l=zeros(n,1);

l(1)=bl(1)-alpha*(-1-0.5*h*w(a+h));
l(n)=bl(n)-beta*cl(n);

for k=2:n-1
l(k)=bl(k);
end

M = Chase_method(A,l);

for k=2:n+1

y(k)=M(k-1);

end

for k=1:n+2
y(k)=y(k)-f(a+(k-1)*h);
end
