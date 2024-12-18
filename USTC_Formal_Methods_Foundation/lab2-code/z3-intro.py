from z3 import *
from pro_print import *

# Z3 is an SMT solver. In this lecture, we'll discuss
# the basis usage of Z3 through some working example, the
# primary goal is to introduce how to use Z3 to solve
# the satisfiability problems we've discussed in the past
# several lectures.
# We must emphasize that Z3 is just one of the many such SMT
# solvers, and by introducing Z3, we hope you will have a
# general understanding of what such solvers look like, and
# what they can do.

########################################
# Basic propositional logic

# In Z3, we can declare two propositions just as booleans, this
# is rather natural, for propositions can have values true or false.
# To declare two propositions P and Q:
P = Bool('P')
Q = Bool('Q')
# or, we can use a more compact shorthand:
P, Q = Bools('P Q')


# We can build propositions by writing Lisp-style abstract
# syntax trees, for example, the disjunction:
# P \/ Q
# can be encoded as the following AST:
F = Or(P, Q)
# Output is : Or(P, Q)
print(F)

# Note that the connective '\/' is called 'Or' in Z3, we'll see
# several other in the next.

# We have designed the function 'pretty_print(expr)' for you, 
# As long as we input the expression defined by z3, we can output 
# propositions that are suitable for us to read.
# Here‘s an example:

P, Q = Bools('P Q')
F = Or(P, Q)

# Output is : P \/ Q
pretty_print(F)

################################################################
##                           Part A                           ##
################################################################

# exercises 1 : P -> (Q -> P)
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P, Q = Bools('P Q')
A1 = Implies(Q, P)
A2 = Implies(P, A1)
solver = Solver()

solver.add(Not(A2))
print(solver.check())

if solver.check() == sat:
    print("The negation is satisfiable, so the proposition is not valid.")
else:
    print("The proposition is valid!")

# exercise 2 : (P -> Q) -> ((Q -> R) -> (P -> R))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')
R = Bool('R')
prop = Implies(Implies(P, Q), Implies(Implies(Q, R), Implies(P, R)))

solver = Solver()

solver.add(Not(prop))
print(solver.check())

if solver.check() == sat:
    print("The negation is satisfiable, so the proposition is not valid.")
else:
    print("The proposition is valid!")

# exercise 3 : (P /\ (Q /\ R)) -> ((P /\ Q) /\ R)
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')
R = Bool('R')

lhs = And(P, And(Q, R))  # (P /\ (Q /\ R))
rhs = And(And(P, Q), R)  # ((P /\ Q) /\ R)
proposition = Implies(lhs, rhs)

solver = Solver()
solver.add(Not(proposition))

print(solver.check())
if solver.check() == sat:
    print("The proposition is NOT valid. Counterexample:")
    print(solver.model())
else:
    print("The proposition is valid!")

# exercise 4 : (P \/ (Q \/ R)) -> ((P \/ Q) \/ R)
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')
R = Bool('R')

lhs = Or(P, Or(Q, R))  # (P \/ (Q \/ R))
rhs = Or(Or(P, Q), R)  # ((P \/ Q) \/ R)
proposition = Implies(lhs, rhs)

solver = Solver()
solver.add(Not(proposition))

print(solver.check())
if solver.check() == sat:
    print("The proposition is NOT valid. Counterexample:")
    print(solver.model())
else:
    print("The proposition is valid!")

# exercise 5 : ((P -> R) /\ (Q -> R)) -> ((P /\ Q) -> R)
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')
R = Bool('R')

lhs = And(Implies(P, R), Implies(Q, R))  # ((P -> R) /\ (Q -> R))
rhs = Implies(And(P, Q), R)  # ((P /\ Q) -> R)
proposition = Implies(lhs, rhs)

solver = Solver()
solver.add(Not(proposition))

print(solver.check())
if solver.check() == sat:
    print("The proposition is NOT valid. Counterexample:")
    print(solver.model())
else:
    print("The proposition is valid!")
    
# exercise 6 : ((P /\ Q) -> R) <-> (P -> (Q -> R))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')
R = Bool('R')

lhs = Implies(And(P, Q), R)  # ((P /\ Q) -> R)
rhs = Implies(P, Implies(Q, R))  # (P -> (Q -> R))
proposition = And(Implies(lhs, rhs), Implies(rhs, lhs))

solver = Solver()
solver.add(Not(proposition))

print(solver.check())
if solver.check() == sat:
    print("The proposition is NOT valid. Counterexample:")
    print(solver.model())
else:
    print("The proposition is valid!")
    
# exercise 7 : (P -> Q) -> (¬Q -> ¬P)
# Please use z3 to define the proposition 
# Note that you need to define the proposition, and prove it.
P = Bool('P')
Q = Bool('Q')

lhs = Implies(P, Q)  # (P -> Q)
rhs = Implies(Not(Q), Not(P))  # (¬Q -> ¬P)
proposition = Implies(lhs, rhs)

solver = Solver()
solver.add(Not(proposition))

print(solver.check())
if solver.check() == sat:
    print("The proposition is NOT valid. Counterexample:")
    print(solver.model())
else:
    print("The proposition is valid!")
################################################################
##                           Part B                           ##
################################################################

# Before writing the src first, we need to understand the
# quantifier. ∀ x.P (x) means that for any x, P (x) holds, 
# so both x and P should be a sort types. IntSort() and BoolSort() 
# are given in Z3
# IntSort(): Return the integer sort in the given context.
# BoolSort(): Return the Boolean Z3 sort.
isort = IntSort()
bsort = BoolSort()
  
# Declare a Int variable x
x = Int('x')

# Declare a function P with input of isort type and output 
# of bsort type
P = Function('P', isort, bsort)

# It means ∃x.P(x)
F = Exists(x, P(x))
print(F)
pretty_print(F)

# Now you can complete the following exercise based on the example above

# exercise 8 : # ∀x.(¬P(x) /\ Q(x)) -> ∀x.(P(x) -> Q(x))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())

# Define the proposition
# ∀x.((¬P(x) /\ Q(x)) -> ∀x.(P(x) -> Q(x)))
lhs = ForAll(x, And(Not(P(x)), Q(x)))
rhs = ForAll(x, Implies(P(x), Q(x)))
prop1 = Implies(lhs, rhs)

# Prove the proposition
s = Solver()
s.add(Not(prop1))  # Add the negation of the proposition to the solver

# Check if the proposition is valid
if s.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", s.model())


# exercise 9 : ∀x.(P(x) /\ Q(x)) <-> (∀x.P(x) /\ ∀x.Q(x))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())

lhs = ForAll(x, And(P(x), Q(x))) # ∀x.(P(x) /\ Q(x))
rhs = And(ForAll(x, P(x)), ForAll(x, Q(x))) # (∀x.P(x) /\ ∀x.Q(x))
proposition = And(Implies(lhs, rhs), Implies(rhs, lhs))

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 10 : ∃x.(¬P(x) \/ Q(x)) -> ∃x.(¬(P(x) /\ ¬Q(x)))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())

lhs = Exists(x, Or(Not(P(x)), Q(x))) # ∃x.(¬P(x) \/ Q(x))
rhs = Exists(x, Not(And(P(x), Not(Q(x))))) # ∃x.(¬(P(x) /\ ¬Q(x)))
proposition = Implies(lhs, rhs)

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 11 : ∃x.(P(x) \/ Q(x)) <-> (∃x.P(x) \/ ∃x.Q(x))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.
x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())

lhs = Exists(x, Or(P(x), Q(x))) # ∃x.(P(x) \/ Q(x)) 
rhs = Or(Exists(x, P(x)), Exists(x, Q(x))) # (∃x.P(x) \/ ∃x.Q(x))
proposition = And(Implies(lhs, rhs), Implies(rhs, lhs))

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 12 : ∀x.(P(x) -> ¬Q(x)) -> ¬(∃x.(P(x) /\ Q(x)))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.

x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())

lhs = ForAll(x, Implies(P(x), Not(Q(x)))) # ∀x.(P(x) -> ¬Q(x))
rhs = Not(Exists(x, And(P(x), Q(x)))) # ¬(∃x.(P(x) /\ Q(x)))
proposition = Implies(lhs, rhs)

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 13 : ∃x.(P(x) /\ Q(x)) /\ ∀x.(P(x) -> R(x)) -> ∃x.(R(x) /\ Q(x))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.

x = Int('x')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), BoolSort())
Q = Function('Q', IntSort(), BoolSort())
R = Function('R', IntSort(), BoolSort())

lhs = And(Exists(x, And(P(x), Q(x))), ForAll(x, Implies(P(x), R(x)))) # ∃x.(P(x) /\ Q(x)) /\ ∀x.(P(x) -> R(x))
rhs = Exists(x, And(R(x), Q(x))) # ∃x.(R(x) /\ Q(x))
proposition = Implies(lhs, rhs)

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 14 : ∃x.∃y.P(x, y) -> ∃y.∃x.P(x, y)
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.

x = Int('x')  # Domain of x is assumed to be integers for demonstration
y = Int('y')  # Domain of x is assumed to be integers for demonstration
P = Function('P', IntSort(), IntSort(), BoolSort())

proposition = Implies(Exists([x, y], P(x, y)), Exists([y, x], P(x, y)))

z = Solver()
z.add(Not(proposition))

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid!")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())

# exercise 15 : P(b) /\ (∀x.∀y.(P(x) /\ P(y) -> x = y)) -> (∀x.(P(x) <-> x = b))
# Please use z3 to define the proposition. 
# Note that you need to define the proposition, and prove it.

# Define the variables
x = Int('x')
y = Int('y')  # Domain of x and y is assumed to be integers for demonstration
b = Int('b')  # Define constant b
P = Function('P', IntSort(), BoolSort())

# Define the proposition
# P(b) /\ (∀x.∀y.(P(x) /\ P(y) -> x = y)) -> (∀x.(P(x) <-> x = b))
proposition = Implies(And(P(b), ForAll([x, y], Implies(And(P(x), P(y)), x == y))), ForAll(x, P(x) == (x == b)))

# Prove the proposition
z = Solver()
z.add(Not(proposition))  # Add the negation of the proposition to the solver

# Check if the proposition is valid
if z.check() == unsat:
    print("The proposition is valid.")
else:
    print("The proposition is not valid.")
    print("Counterexample:", z.model())



################################################################
##                           Part C                           ##
################################################################

# Challenge: 
# We provide the following two rules :
#     ----------------(odd_1)
#           odd 1
#
#           odd n
#     ----------------(odd_ss)
#         odd n + 2
#
# Please prove that integers 9, 25, and 99 are odd numbers.


# Define the rules for odd numbers
odd = Function('odd', IntSort(), BoolSort())
n = Int('n')

# Rule (odd_1): odd 1
rule_odd_1 = odd(1)

# Rule (odd_ss): odd n -> odd (n + 2)
rule_odd_ss = ForAll(n, Implies(odd(n), odd(n + 2)))

# Add rules to the solver
s_odd = Solver()
s_odd.add(rule_odd_1)
s_odd.add(rule_odd_ss)

# Prove that 9, 25, and 99 are odd
nums_to_prove = [9, 25, 99]
for num in nums_to_prove:
    s_odd.push()  # Save the current state of the solver
    s_odd.add(Not(odd(num)))  # Negate the statement "odd(num)"
    if s_odd.check() == unsat:
        print(f"{num} is odd.")
    else:
        print(f"{num} is not odd.")
        print("Counterexample:", s_odd.model())
    s_odd.pop()  # Restore the solver state



