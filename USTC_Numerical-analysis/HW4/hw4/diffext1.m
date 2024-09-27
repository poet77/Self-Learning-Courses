function [Dy,dy,n] =diffext1 (fun,x0,max)
h=1;j=1; n=1; jdW=1; xdW=1;
x1=x0+h;
x2=x0-h;
Dy(1,1)=(feval(fun,x1)-feval(fun,x2))/(2*h);

while(j<max)
    x1=x0+2^(-j)*h;
    x2=x0-2^(-j)*h;
    Dy(j+1,1)=(feval(fun,x1)-feval(fun,x2))/(2^(1-j)*h);
    for k=1:j
        Dy(j+1,k+1)=Dy(j+1,k)+(Dy(j+1,k)-Dy(j,k))/(4^k-1);
    end
    jdW=abs(Dy(j+1,j+1)-Dy(j+1,j));
    j=j+1;
end

[n,n]=size(Dy);
dy=Dy(n,n);