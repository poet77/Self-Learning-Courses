% This function is used to get the feasible point
% The question is:
% min s
% s.t. Cx = d
% and g(x) <=s
% Using Newton descent with backtracking line search 
%First change the original inequalities, g(x)<=0 to g(x)-x(end)<=0, where x(end) is the last variable s

function [x,feasibility] = Firstphase_method(f,C,x0_feasible, diff_eps, stopping_eps, alpha, beta)

t=0.01;
mu=20;
tempx=[];
m=size(f,2)-1; 

temp=-1e8; %Initial value

for i=2:size(f,2) %Set s slighter higher than max of g_is.
    temp=max(temp,f{i}(x0_feasible));
end

x0_feasible=[x0_feasible;temp+1e-1]; %Set slack variable as an extra variable.
C=[C,0]; 

%Reset the function
f_c=f; 
f_c{1}=@(x)(x(end)); 

for i=2:size(f,2)
    f_c{i}=@(x)(f{i}(x(1:(size(x,1)-1)))-x(end)); 
end


f=f_c;
x=x0_feasible;
feasibility=1; %Check if the problem is feasible. 

%Quit when s<0, as we have found strictly feasible point
while(x(end)>=0) 
    x=x0_feasible;
    tempx=[tempx,x];
    while(1)      
        g=modified_gradient(f,x,t,diff_eps);
        H=modified_hessian(f,x,t,diff_eps);
        KKT_matrix=[H,C';C,zeros(size(C,1),size(H,1)+size(C,1)-size(C,2))]; 
        y=[-g;zeros(size(C,1),1)];
        sol=KKT_matrix\y;
        descent=sol(1:size(C,2));
        step_size=1;
        while (1)
            fail=0; %Check goldstein condition, as well as whether within domain of log function
            for m=2:size(f,2)
                if (f{m}(x+step_size*descent)>=0)
                    fail=1;
                end
            end
            if (modified_f(f,x+step_size*descent,t))>(modified_f(f,x,t)+alpha*step_size*descent'*modified_gradient(f,x,t,diff_eps))
                fail=1;   
            end
            if fail==1
               step_size=step_size*beta;
            else
                break
            end      
        end

        %step_size
        x_new=x+step_size*descent;
        
        if(abs(x_new-x)<stopping_eps)
            break
        end
        x=x_new;
     end
    
    if t>(m/stopping_eps)
        feasibility=0;
        break
    else
        t=mu*t;
        x0_feasible=x;
        x(end);
    end
    end
end

% Get the Gradient,Hessian,Value of the function t*f(x)-sum_i log(-g(x_i) 

function [hess]=modified_hessian(f,x,t,epsilon) %Hessian of the function t*f(x) -sum_i log(-g(x_i))

hess=t*Hessian(f{1},x,epsilon); %Hessian of the objective function

    for m=2:size(f,2) 
        hess=hess+(1/((f{m}(x))^2))*Gradient(f{m},x,epsilon)*Gradient(f{m},x,epsilon)' + (-1/(f{m}(x)))*Hessian(f{m},x,epsilon);
    end

 end

function [grad]=modified_gradient(f,x,t,epsilon) %Gradient of the function t*f(x) -sum_i log(-g(x_i))

    grad=t*Gradient(f{1},x,epsilon);  %Gradient of the objective function
    
    for m=2:size(f,2)  
        grad=grad+(-1/(f{m}(x)))*(Gradient(f{m},x,epsilon));
    end
end

function [val]=modified_f(f,x,t) %Value of the function t*f(x) -sum_i log(-g(x_i))

    val=t*f{1}(x);
    for m=2:size(f,2) 
        val=val+ (-1)*log(-f{m}(x));   
    end
end
