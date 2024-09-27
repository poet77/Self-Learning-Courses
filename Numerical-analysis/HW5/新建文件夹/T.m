function Tn=T(f,a,b,n) 
    h=(b-a)/n;
    sum=0;
    for k=1:n-1
        x=a+h*k;
        sum=sum+feval('f',x);
    end

    format long
 
    Tn=(feval('f',a)+2*sum+feval('f',b))*h/2;
end
