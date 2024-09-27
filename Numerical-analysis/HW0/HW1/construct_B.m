phi=zeros(1,301);
S=zeros(1,10);
K=zeros(1,31);

B=zeros(10,3);

B(1,1)=0;

B(1,2)=pi*pi/6;

for x = 1 : 1 : 9

sum = 0;



  for k = 1:100000
	
  sum = sum + 1/(k*(k+0.1*x)*(k+1)*(k+2));
  
  end
 
 
 
B(x+1,1)= x*0.1 ;

B(x+1,2)=(sum*(2-0.1*x)+0.25)*(1-0.1*x)+1;

end
	

for i = 1:10

s=1;

S(i)=B(i,2);

while(S(i)>=1E-6)
	
S(i)=S(i)-1/(s*(s+0.1*(i-1)));

s=s+1;

end
	
B(i,3)=s-1;

end

B
	



	
