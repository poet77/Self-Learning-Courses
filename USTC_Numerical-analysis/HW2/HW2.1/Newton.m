function y = Newton(X,Y,x)

n=length(X); m=length(x);
for t=1:m
    z=x(t); A=zeros(n,n);
    A(:,1)=Y';
    s=0.0; 
    for  j=2:n
       for i=j:n
           A(i,j)=(A(i,j-1)- A(i-1,j-1))/(X(i)-X(i-j+1));
       end
    end
    C=A(n,n);
    for k=1:n
        p=1.0;
        for j=1:k-1
            p=p*(z-X(j));
        end
        s=s+A(k,k)*p;        
    end
    ss(t)=s;
end
    y=ss;  
end


