function z = Lagrange(x,f,x0)

n = length(x) ;

m = length(x0);

tt=zeros(m,1);

    for i = 1:m
    D = x0(i);
    y = 0.0;
    for k = 1:n

    l = 1.0;
        for j = 1:n
            
            if j~=k
            l = l*(D-x(j))/(x(k)-x(j));
            end
        end
     
        y = y + l*f(k);
    end
        tt(i)=y;
        
    end
    z=tt;
end
