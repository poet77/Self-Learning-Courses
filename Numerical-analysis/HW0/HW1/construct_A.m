A=zeros(31,3);
phi=zeros(1,301);
S=zeros(1,301);
K=zeros(1,31);
A(1,1)=1;
A(1,2)=1;
i=1;
j=1;
t=1;
phi(1)=1;
S(1)=1;

for i=2:300
	
phi(i)=induction(phi(i-1),i-1);
	
   if i == 10*j
   j=j+1;
   A(j,1)=i;
   A(j,2)=phi(i);
  
   end	
end
	
s=1;
	
S(1)=phi(1);

while (S(1)>=1E-6)

S(1)=S(1)-1/(s*(s+1));

s=s+1;

end
	
A(1,3)=s-1;

for i=2:300
	
	
S(i)=phi(i);

s=1;

if i==10*t
	

	
while (S(i)>=1E-6)
	
S(i)=S(i)-1/(s*(s+i));

s=s+1;

end
	
A(t+1,3)=s-1;

t=t+1;


end

end
	
A

  
  
  
  
  
  
  
  
  
  
  