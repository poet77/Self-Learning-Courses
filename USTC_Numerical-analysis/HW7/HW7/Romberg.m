function [Dy,dy,n] =Romberg(f,a,b,M)
h=b-a;
Dy(1,1)=0.5*h*(f(a)+f(b));



for n=2:M+1
    h=h/2;
    Dy(n,1)=0.5*Dy(n-1,1);
    for i=1:2^(n-2)
        Dy(n,1)=Dy(n,1)+h*f(a+(2*i-1)*h);
    end

    for m=2:n
        
        Dy(n,m)=Dy(n,m-1)+(Dy(n,m-1)-Dy(n-1,m-1))/(4^(m-1)-1);

    end


end


[k,k]=size(Dy);
dy=Dy(k,k);