% This function is used to do the Newton method
%This is an implementation of the barrier method with the log barrier
%function.  Refer Convex optimization by Boyd, page 569.
% we minimize t*f(x) + phi, for increasing values of t.

function [x,count] = Newton_method(f,C,x0_feasible, diff_eps, stopping_eps, alpha, beta)


t=0.1; 
mu=10; 
count =0;

%tempx=[];
m=size(f,2)-1; %number of inequality constraints
while(1)
    x=x0_feasible;
    count = count+1;
 %   tempx=[tempx,x0_feasible];
    %This is the beginning of a centering step.
    while(1)
        
        g=modified_gradient(f,x,t,diff_eps);
        H=modified_hessian(f,x,t,diff_eps);
        if rank(H)<size(H,1)
            H=H+eye(size(H,1))*1e-4; 
        end
        KKT_matrix=[H,C';C,zeros(size(C,1),size(H,1)+size(C,1)-size(C,2))]; %KKT matrix
        y=[-g;zeros(size(C,1),1)];
     %   if rank(KKT_matrix)<4
      %      disp(KKT_matrix)
     %   end
        sol=KKT_matrix\y;
        descent=sol(1:size(C,2));
        step_size=1;
        
        while (1)
            fail=0; %check goldstein condition, as well as whether within domain of log function
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
        
        x_new=x+step_size*descent;
        if(norm(x_new-x,inf)<stopping_eps)
            break
        end
        x=x_new;
     end
    
    if t>(m/stopping_eps)
        break
    else
        t=mu*t;%go to next centering step
        x0_feasible=x;
        
    end
end

fprintf('The optimal value as %d and the solution is \n',f{1}(x))
disp(x);
fprintf('The number of iterations is %d \n',count)
end

function [hess]=modified_hessian(f,x,t,epsilon) %hessian of the function t*f(x) + -sum_i log(-g(x_i))

hess=t*Hessian(f{1},x,epsilon); %hessian of t*objective
    for m=2:size(f,2) 
        hess=hess+(1/((f{m}(x)+1e-20)^2))*Gradient(f{m},x,epsilon)*Gradient(f{m},x,epsilon)' + (-1/(f{m}(x)+1e-20))*Hessian(f{m},x,epsilon);
    end

 end

function [grad]=modified_gradient(f,x,t,epsilon) %gradient of the function t*f(x) + -sum_i log(-g(x_i))

    grad=t*Gradient(f{1},x,epsilon); %gradient of t*objective
    
    for m=2:size(f,2)  
        grad=grad+(-1/(f{m}(x)+1e-20))*(Gradient(f{m},x,epsilon));
    end
end

function [val]=modified_f(f,x,t)

    val=t*f{1}(x); %value of t*objective
    for m=2:size(f,2) 
        val=val+ (-1)*log(-f{m}(x));

    end
end