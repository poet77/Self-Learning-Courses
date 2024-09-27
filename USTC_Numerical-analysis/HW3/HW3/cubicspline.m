function Sx = cubicspline(x,y,x0)

n=length(x);


for k=2:n
    h(k)=x(k)-x(k-1);
end

for i=2:n-1
    lambda(i)=h(i+1)/(h(i)+h(i+1));
    mu(i)=1-lambda(i);
    d(i)=6/(h(i)+h(i+1))*((y(i+1)-y(i))/h(i+1)-(y(i)-y(i-1))/h(i));
end


A=zeros(n,n);
A(1,1)=2;
A(n,n)=2;
for k=2:n-1
    A(k,k)=2;
    A(k,k+1)=lambda(k);
    A(k,k-1)=mu(k);
end
b=zeros(n,1);


    dy1_0 = 1;
    dy1_n = exp(1);
    
   A(1,2)=1;
    A(n,n-1)=1;
    b(1)=6/h(2)*((y(2)-y(1))/h(2)-dy1_0);
    b(n)=6/h(n)*(dy1_n-(y(n)-y(n-1))/h(n));


for k=2:(n-1)
    b(k,1)=d(k);
end


M = Chase_method(A,b);

for i=1:n-1
    if (x0<x(i+1))&&(x0>=x(i))

    Sx = (((x(i+1)-x0)^3)*M(i)+((x0-x(i))^3)*M(i+1))/(6*h(i+1)) + ...
    (y(i)-M(i)*(h(i+1)^2)/6)*(x(i+1)-x0)/h(i+1) + (y(i+1)-M(i+1)*(h(i+1)^2)/6)*(x0-x(i))/h(i+1);
    
end
end
