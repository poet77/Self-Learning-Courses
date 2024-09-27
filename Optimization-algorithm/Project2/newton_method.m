function x_star = newton_method(f, grad_f, hess_f, eq_constraint, grad_eq_constraint, x0, epsilon, max_iter)
    x = x0;
    iter = 0;
    
    while iter < max_iter
        grad = grad_f(x);
        hess = hess_f(x);
        eq_grad = grad_eq_constraint(x);
        eq_val = eq_constraint(x);
        
      
kkt_matrix = [hess, eq_grad'; eq_grad, zeros(size(eq_grad, 2))];
        kkt_rhs = [-grad; zeros(size(eq_grad, 1), 1)];
        
        delta = kkt_matrix \ kkt_rhs;
        delta_x = delta(1:length(x));
        
        lambda = delta(length(x) + 1:end);
        lambda_norm = norm(lambda);
        
        if lambda_norm^2 / 2 <= epsilon
            break;
        end
        
        t = 1;
        alpha = 0.25;
        beta = 0.5;
        
        while f(x + t * delta_x) > f(x) + alpha * t * grad' * delta_x
            t = beta * t;
        end
        
        x = x + t * delta_x;
        
        iter = iter + 1;
    end
    
    x_star = x;
end