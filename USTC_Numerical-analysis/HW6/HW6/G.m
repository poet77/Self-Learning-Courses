function Gauss=G(f,a,b,n) 
h=(b-a)/n;
  sum=0;
 ga=sqrt(3/5);
  for k=1:n
      x=a+k*h;
      y=x-h;
      sum=sum+(5*f((x+y)/2-(x-y)*ga/2)+8*f((x+y)/2)+5*f((x+y)/2+(x-y)*ga/2))*(x-y)/18;
  end
Gauss=sum;
end