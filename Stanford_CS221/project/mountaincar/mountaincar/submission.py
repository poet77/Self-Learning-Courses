import math, random
from collections import defaultdict
from typing import List, Callable, Tuple, Dict, Any, Optional, Iterable, Set
import gymnasium as gym
import numpy as np
from itertools import product

import util
from util import ContinuousGymMDP, StateT, ActionT
from custom_mountain_car import CustomMountainCarEnv

############################################################
# Problem 3a
# Implementing Value Iteration on Number Line (from Problem 1)
def valueIteration(succAndRewardProb: Dict[Tuple[StateT, ActionT], List[Tuple[StateT, float, float]]], discount: float, epsilon: float = 0.001):
    '''
    Given transition probabilities and rewards, computes and returns V and
    the optimal policy pi for each state.
    - succAndRewardProb: Dictionary mapping tuples of (state, action) to a list of (nextState, prob, reward) Tuples.
    - Returns: Dictionary mapping each state to an action.
    '''
    # Define a mapping from states to Set[Actions] so we can determine all the actions that can be taken from s.
    # You may find this useful in your approach.
    stateActions = defaultdict(set)
    for state, action in succAndRewardProb.keys():
        stateActions[state].add(action)

    def computeQ(V: Dict[StateT, float], state: StateT, action: ActionT) -> float:
        # Return Q(state, action) based on V(state)
        # BEGIN_YOUR_CODE (our solution is 2 lines of code, but don't worry if you deviate from this)
        total = 0.0
        for nextState, prob, reward in succAndRewardProb[(state, action)]:
            total += prob * (reward + discount * V[nextState])
        return total

        # END_YOUR_CODE

    def computePolicy(V: Dict[StateT, float]) -> Dict[StateT, ActionT]:
        # Return the policy given V.
        # Remember the policy for a state is the action that gives the greatest Q-value.
        # IMPORTANT: if multiple actions give the same Q-value, choose the largest action number for the policy. 
        # HINT: We only compute policies for states in stateActions.
        # BEGIN_YOUR_CODE (our solution is 4 lines of code, but don't worry if you deviate from this)
        policy = {}
        for state in stateActions.keys():
            best_action = None
            best_q_value = -100.0
            for action in stateActions[state]:
                q_value = computeQ(V, state, action)
                #print(q_value)
                #print(best_q_value)
                if (q_value > best_q_value) or (q_value == best_q_value and (best_action is None or action > best_action)):
                    best_q_value = q_value
                    best_action = action
            policy[state] = best_action
            #print(policy[state])

        return policy
        # END_YOUR_CODE

    print('Running valueIteration...')
    V = defaultdict(float) # This will return 0 for states not seen (handles terminal states)
    numIters = 0
    while True:
        newV = defaultdict(float) # This will return 0 for states not seen (handles terminal states)
        # update V values using the computeQ function above.
        # repeat until the V values for all states converge (changes between iterations are less than epsilon).
        # BEGIN_YOUR_CODE (our solution is 4 lines of code, but don't worry if you deviate from this)
        delta = 0.0
        for state in stateActions.keys():
            best_q_value = float('-inf')
            for action in stateActions[state]:
                q_value = computeQ(V, state, action)
                best_q_value = max(best_q_value, q_value)
            newV[state] = best_q_value  # Update new value for the state
            delta = max(delta, abs(newV[state] - V[state]))  # Calculate the maximum change
        if delta < epsilon:  # Check for convergence
            break
        # END_YOUR_CODE
        V = newV
        numIters += 1
    V_opt = V
    print(("valueIteration: %d iterations" % numIters))
    return computePolicy(V_opt)

############################################################
# Problem 3b
# Model-Based Monte Carlo

# Runs value iteration algorithm on the number line MDP
# and prints out optimal policy for each state.
def run_VI_over_numberLine(mdp: util.NumberLineMDP):
    succAndRewardProb = {
        (-mdp.n + 1, 1): [(-mdp.n + 2, 0.2, mdp.penalty), (-mdp.n, 0.8, mdp.leftReward)],
        (-mdp.n + 1, 2): [(-mdp.n + 2, 0.3, mdp.penalty), (-mdp.n, 0.7, mdp.leftReward)],
        (mdp.n - 1, 1): [(mdp.n - 2, 0.8, mdp.penalty), (mdp.n, 0.2, mdp.rightReward)],
        (mdp.n - 1, 2): [(mdp.n - 2, 0.7, mdp.penalty), (mdp.n, 0.3, mdp.rightReward)]
    }

    for s in range(-mdp.n + 2, mdp.n - 1):
        succAndRewardProb[(s, 1)] = [(s+1, 0.2, mdp.penalty), (s - 1, 0.8, mdp.penalty)]
        succAndRewardProb[(s, 2)] = [(s+1, 0.3, mdp.penalty), (s - 1, 0.7, mdp.penalty)]

    pi = valueIteration(succAndRewardProb, mdp.discount)
    return pi


class ModelBasedMonteCarlo(util.RLAlgorithm):
    def __init__(self, actions: List[ActionT], discount: float, calcValIterEvery: int = 10000,
                 explorationProb: float = 0.2,) -> None:
        self.actions = actions
        self.discount = discount
        self.calcValIterEvery = calcValIterEvery
        self.explorationProb = explorationProb
        self.numIters = 0

        # (state, action) -> {nextState -> ct} for all nextState
        self.tCounts = defaultdict(lambda: defaultdict(int))
        # (state, action) -> {nextState -> totalReward} for all nextState
        self.rTotal = defaultdict(lambda: defaultdict(float))

        self.pi = {} # Optimal policy for each state. state -> action

    # This algorithm will produce an action given a state.
    # Here we use the epsilon-greedy algorithm: with probability |explorationProb|, take a random action.
    # Should return random action if the given state is not in self.pi.
    # The input boolean |explore| indicates whether the RL algorithm is in train or test time. If it is false (test), we
    # should always follow the policy if available.
    # HINT: Use random.random() (not np.random()) to sample from the uniform distribution [0, 1]
    def getAction(self, state: StateT, explore: bool = True) -> ActionT:
        if explore:
            self.numIters += 1
        explorationProb = self.explorationProb
        if self.numIters < 2e4: # Always explore
            explorationProb = 1.0
        elif self.numIters > 1e6: # Lower the exploration probability by a logarithmic factor.
            explorationProb = explorationProb / math.log(self.numIters - 100000 + 1)
        # BEGIN_YOUR_CODE (our solution is 5 lines of code, but don't worry if you deviate from this)
        if random.random() < explorationProb:
            return random.choice(self.actions)
        else:
            return self.pi.get(state, random.choice(self.actions))  # Fallback to random action if no policy
        # END_YOUR_CODE

    # We will call this function with (s, a, r, s'), which is used to update tCounts and rTotal.
    # For every self.calcValIterEvery steps, runs value iteration after estimating succAndRewardProb.
    def incorporateFeedback(self, state: StateT, action: ActionT, reward: int, nextState: StateT, terminal: bool):

        self.tCounts[(state, action)][nextState] += 1
        self.rTotal[(state, action)][nextState] += reward

        if self.numIters % self.calcValIterEvery == 0:
            # Estimate succAndRewardProb based on self.tCounts and self.rTotal.
            # Hint 1: prob(s, a, s') = (counts of transition (s,a) -> s') / (total transtions from (s,a))
            # Hint 2: Reward(s, a, s') = (total reward of (s,a) -> s') / (counts of transition (s,a) -> s')
            # Then run valueIteration and update self.pi.
            succAndRewardProb = defaultdict(list)
            # BEGIN_YOUR_CODE (our solution is 7 lines of code, but don't worry if you deviate from this)
                    # Calculate transition probabilities and expected rewards
            for (s, a), next_states in self.tCounts.items():
                total_count = sum(next_states.values())
                for next_state, count in next_states.items():
                    # Probability of transitioning to next_state given (s, a)
                    prob = count / total_count
                    # Expected reward for transitioning to next_state given (s, a)
                    total_reward = self.rTotal[(s, a)][next_state]
                    expected_reward = total_reward / count if count > 0 else 0
                    succAndRewardProb[(s, a)].append((next_state, prob, expected_reward))

            # Run value iteration to update self.pi
            self.pi = valueIteration(succAndRewardProb, self.discount)
            # END_YOUR_CODE

############################################################
# Problem 4a
# Performs Tabular Q-learning. Read util.RLAlgorithm for more information.
class TabularQLearning(util.RLAlgorithm):
    def __init__(self, actions: List[ActionT], discount: float,
                 explorationProb: float = 0.2, initialQ: float = 0):
        '''
        - actions: the list of valid actions
        - discount: a number between 0 and 1, which determines the discount factor
        - explorationProb: the epsilon value indicating how frequently the policy returns a random action
        - intialQ: the value for intializing Q values.
        '''
        self.actions = actions
        self.discount = discount
        self.explorationProb = explorationProb
        self.Q = defaultdict(lambda: initialQ)
        self.numIters = 0

    # This algorithm will produce an action given a state.
    # Here we use the epsilon-greedy algorithm: with probability |explorationProb|, take a random action.
    # The input boolean |explore| indicates whether the RL algorithm is in train or test time. If it is false (test), we
    # should always choose the maximum Q-value action.
    # HINT 1: You can access Q-value with self.Q[state, action]
    # HINT 2: Use random.random() to sample from the uniform distribution [0, 1]
    def getAction(self, state: StateT, explore: bool = True) -> ActionT:
        if explore:
            self.numIters += 1
            explorationProb = self.explorationProb
            if self.numIters < 2e4: # explore
                explorationProb = 1.0
            elif self.numIters > 1e5: # Lower the exploration probability by a logarithmic factor.
                explorationProb = explorationProb / math.log(self.numIters - 100000 + 1)
            # BEGIN_YOUR_CODE (our solution is 5 lines of code, but don't worry if you deviate from this)
            if random.random() < explorationProb:
                return random.choice(self.actions)
            else:
                return max(self.actions, key=lambda action: self.Q[(state, action)])
        else:
        # During testing, always choose the maximum Q-value action
            return max(self.actions, key=lambda action: self.Q[(state, action)])
        # END_YOUR_CODE

    # Call this function to get the step size to update the weights.
    def getStepSize(self) -> float:
        return 0.1

    # We will call this function with (s, a, r, s'), which you should use to update |Q|.
    # Note that if s is a terminal state, then terminal will be True.  Remember to check for this.
    # You should update the Q values using self.getStepSize() 
    # HINT 1: The target V for the current state is a combination of the immediate reward
    # and the discounted future value.
    # HINT 2: V for terminal states is 0
    def incorporateFeedback(self, state: StateT, action: ActionT, reward: float, nextState: StateT, terminal: bool) -> None:
        # BEGIN_YOUR_CODE (our solution is 8 lines of code, but don't worry if you deviate from this)
            # Calculate the target value V
        target_value = reward  # Initialize target value to the current reward
        
        if not terminal:
            # If not a terminal state, calculate the maximum Q value for the next state
            max_future_q = max(self.Q[(nextState, a)] for a in self.actions)
            target_value += self.discount * max_future_q  # Add the discounted future Q value to the target value
        # Update Q value using adaptive learning rate
        step_size = self.getStepSize()
        self.Q[(state, action)] += step_size * (target_value - self.Q[(state, action)])
        # END_YOUR_CODE

############################################################
# Problem 4b: Fourier feature extractor

def fourierFeatureExtractor(
        state: StateT,
        maxCoeff: int = 5,
        scale: Optional[Iterable] = None
    ) -> np.ndarray:
    '''
    For state (x, y, z), maxCoeff 2, and scale [2, 1, 1], this should output (in any order):
    [1, cos(pi * 2x), cos(pi * y), cos(pi * z),
     cos(pi * (2x + y)), cos(pi * (2x + z)), cos(pi * (y + z)),
     cos(pi * (4x)), cos(pi * (2y)), cos(pi * (2z)),
     cos(pi*(4x + y)), cos(pi * (4x + z)), ..., cos(pi * (4x + 2y + 2z))]
    '''
    if scale is None:
        scale = np.ones_like(state)
    features = None
    state = np.array(state)
    scale = np.array(scale) if scale is not None else np.ones_like(state)

    # Below, implement the fourier feature extractor as similar to the doc string provided.
    # The return shape should be 1 dimensional ((maxCoeff+1)^(len(state)),).
    #
    # HINT: refer to util.polynomialFeatureExtractor as a guide for
    # doing efficient arithmetic broadcasting in numpy.

    # BEGIN_YOUR_CODE (our solution is 7 lines of code, but don't worry if you deviate from this)
        # Generate the combinations of coefficients
    coeffs = range(maxCoeff + 1)
    combinations = list(product(coeffs, repeat=len(state)))

    # Calculate the features
    features = []
    for combo in combinations:
        # Compute the weighted sum of state variables based on the coefficients
        weighted_sum = sum(c * s for c, s in zip(combo, state * scale))
        # Append the cosine of pi times the weighted sum
        features.append(np.cos(np.pi * weighted_sum))

    return np.array(features)
    # END_YOUR_CODE

    #return features

############################################################
# Problem 4c: Q-learning with Function Approximation
# Performs Function Approximation Q-learning. Read util.RLAlgorithm for more information.
class FunctionApproxQLearning(util.RLAlgorithm):
    def __init__(self, featureDim: int, featureExtractor: Callable, actions: List[int],
                 discount: float, explorationProb=0.2):
        '''
        - featureDim: the dimensionality of the output of the feature extractor
        - featureExtractor: a function that takes a state and returns a numpy array representing the feature.
        - actions: the list of valid actions
        - discount: a number between 0 and 1, which determines the discount factor
        - explorationProb: the epsilon value indicating how frequently the policy returns a random action
        '''
        self.featureDim = featureDim
        self.featureExtractor = featureExtractor
        self.actions = actions
        self.discount = discount
        self.explorationProb = explorationProb
        self.W = np.random.standard_normal(size=(featureDim, len(actions)))
        self.numIters = 0

    def getQ(self, state: np.ndarray, action: int) -> float:
        # BEGIN_YOUR_CODE (our solution is 3 lines of code, but don't worry if you deviate from this)
        # Calculate the Q-value for a given state and action using the weight vector W.
        features = self.featureExtractor(state)
        return np.dot(features, self.W[:, action])
        # END_YOUR_CODE

    # This algorithm will produce an action given a state.
    # Here we use the epsilon-greedy algorithm: with probability |explorationProb|, take a random action.
    # The input boolean |explore| indicates whether the RL algorithm is in train or test time. If it is false (test), we
    # should always choose the maximum Q-value action.
    # HINT: This function should be the same as your implementation for 4a.
    def getAction(self, state: np.ndarray, explore: bool = True) -> int:
        if explore:
            self.numIters += 1
        explorationProb = self.explorationProb
        if self.numIters < 2e4: # Always explore
            explorationProb = 1.0
        elif self.numIters > 1e5: # Lower the exploration probability by a logarithmic factor.
            explorationProb = explorationProb / math.log(self.numIters - 100000 + 1)

        # BEGIN_YOUR_CODE (our solution is 5 lines of code, but don't worry if you deviate from this)
        if np.random.random() < explorationProb:
            return np.random.choice(self.actions)
        else:
            # Choose the action with the maximum Q-value
            q_values = [self.getQ(state, action) for action in self.actions]
            return np.argmax(q_values)
        # END_YOUR_CODE

    # Call this function to get the step size to update the weights.
    def getStepSize(self) -> float:
        return 0.005 * (0.99)**(self.numIters / 500)

    # We will call this function with (s, a, r, s'), which you should use to update |weights|.
    # Note that if s is a terminal state, then terminal will be True.  Remember to check for this.
    # You should update W using self.getStepSize()
    # HINT 1: this part will look similar to 4a, but you are updating self.W
    # HINT 2: review the function approximation module for the update rule
    def incorporateFeedback(self, state: np.ndarray, action: int, reward: float, nextState: np.ndarray, terminal: bool) -> None:
        # BEGIN_YOUR_CODE (our solution is 8 lines of code, but don't worry if you deviate from this)
        features = self.featureExtractor(state)
        if terminal:
            target = reward  # For terminal states, V is 0
        else:
            next_features = self.featureExtractor(nextState)
            target = reward + self.discount * np.dot(next_features, self.W[:, np.argmax([self.getQ(nextState, a) for a in self.actions])])

        # Update the weights
        step_size = self.getStepSize()
        self.W[:, action] += step_size * (target - self.getQ(state, action)) * features
        # END_YOUR_CODE

############################################################
# Problem 5c: Constrained Q-learning

class ConstrainedQLearning(FunctionApproxQLearning):
    def __init__(self, featureDim: int, featureExtractor: Callable, actions: List[int],
                 discount: float, force: float, gravity: float,
                 max_speed: Optional[float] = None,
                 explorationProb=0.2):
        super().__init__(featureDim, featureExtractor, actions,
                         discount, explorationProb)
        self.force = force
        self.gravity = gravity
        self.max_speed = max_speed

    # This algorithm will produce an action given a state.
    # Here we use the epsilon-greedy algorithm: with probability |explorationProb|, take a random action.
    # The input boolean |explore| indicates whether the RL algorithm is in train or test time. If it is false (test), we
    # should always choose the maximum Q-value action that is valid.
    def getAction(self, state: np.ndarray, explore: bool = True) -> int:
        if explore:
            self.numIters += 1
        explorationProb = self.explorationProb
        if self.numIters < 2e4: # Always explore
            explorationProb = 1.0
        elif self.numIters > 1e5: # Lower the exploration probability by a logarithmic factor.
            explorationProb = explorationProb / math.log(self.numIters - 100000 + 1)

        # BEGIN_YOUR_CODE (our solution is 15 lines of code, but don't worry if you deviate from this)
        raise Exception("Not implemented yet")
        # END_YOUR_CODE

############################################################
# This is helper code for comparing the predicted optimal
# actions for 2 MDPs with varying max speed constraints
gym.register(
    id="CustomMountainCar-v0",
    entry_point="custom_mountain_car:CustomMountainCarEnv",
    max_episode_steps=1000,
    reward_threshold=-110.0,
)

mdp1 = ContinuousGymMDP("CustomMountainCar-v0", discount=0.999, timeLimit=1000)
mdp2 = ContinuousGymMDP("CustomMountainCar-v0", discount=0.999, timeLimit=1000)

# This is a helper function for 5c. This function runs
# ConstrainedQLearning, then simulates various trajectories through the MDP
# and compares the frequency of various optimal actions.
def compare_MDP_Strategies(mdp1: ContinuousGymMDP, mdp2: ContinuousGymMDP):
    rl1 = ConstrainedQLearning(
        36,
        lambda s: fourierFeatureExtractor(s, maxCoeff=5, scale=[1, 15]),
        mdp1.actions,
        mdp1.discount,
        mdp1.env.force,
        mdp1.env.gravity,
        10000,
        explorationProb=0.2,
    )
    rl2 = ConstrainedQLearning(
        36,
        lambda s: fourierFeatureExtractor(s, maxCoeff=5, scale=[1, 15]),
        mdp2.actions,
        mdp2.discount,
        mdp2.env.force,
        mdp2.env.gravity,
        0.065,
        explorationProb=0.2,
    )
    sampleKRLTrajectories(mdp1, rl1)
    sampleKRLTrajectories(mdp2, rl2)

def sampleKRLTrajectories(mdp: ContinuousGymMDP, rl: ConstrainedQLearning):
    accelerate_left, no_accelerate, accelerate_right = 0, 0, 0
    for n in range(100):
        traj = util.sample_RL_trajectory(mdp, rl)
        accelerate_left = traj.count(0)
        no_accelerate = traj.count(1)
        accelerate_right = traj.count(2)

    print(f"\nRL with MDP -> start state:{mdp.startState()}, max_speed:{rl.max_speed}")
    print(f"  *  total accelerate left actions: {accelerate_left}, total no acceleration actions: {no_accelerate}, total accelerate right actions: {accelerate_right}")
