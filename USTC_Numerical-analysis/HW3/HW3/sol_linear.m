function y= sol_linear(x0,y0,x,n)

ss=0;

   for i=1:n-1

    if(x>=x0(i))&&(x<=x0(i+1))

        ss=(x-x0(i+1))/(x0(i)-x0(i+1))*y0(i)+(x-x0(i))/(x0(i+1)-x0(i))*y0(i+1);
    else
        continue;

    end

   end

y=ss;

end



