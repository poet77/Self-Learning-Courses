#!/usr/bin/python

import random
import collections
from typing import Callable, Dict, List, Tuple, TypeVar, DefaultDict

from util import *

FeatureVector = Dict[str, int]
WeightVector = Dict[str, float]
Example = Tuple[FeatureVector, int]



############################################################
# Problem 3: binary classification
############################################################

############################################################
# Problem 3a: feature extraction


def extractWordFeatures(x: str) -> FeatureVector:
    """
    Extract word features for a string x. Words are delimited by
    whitespace characters only.
    @param string x:
    @return dict: feature vector representation of x.
    Example: "I am what I am" --> {'I': 2, 'am': 2, 'what': 1}
    """
    # BEGIN_YOUR_CODE (our solution is 4 lines of code, but don't worry if you deviate from this)
    word_count = collections.defaultdict(int)
    for word in x.split():
        word_count[word] += 1
    return dict(word_count)

    # END_YOUR_CODE


############################################################
# Problem 3b: stochastic gradient descent

T = TypeVar('T')


def learnPredictor(trainExamples: List[Tuple[T, int]],
                   validationExamples: List[Tuple[T, int]],
                   featureExtractor: Callable[[T], FeatureVector],
                   numEpochs: int, eta: float) -> WeightVector:
    '''
    Given |trainExamples| and |validationExamples| (each one is a list of (x,y)
    pairs), a |featureExtractor| to apply to x, and the number of epochs to
    train |numEpochs|, the step size |eta|, return the weight vector (sparse
    feature vector) learned.

    You should implement stochastic gradient descent.

    Notes:
    - Only use the trainExamples for training!
    - You should call evaluatePredictor() on both trainExamples and
      validationExamples to see how you're doing as you learn after each epoch.
    - The predictor should output +1 if the score is precisely 0.
    '''
    weights = {}  # feature => weight

    def predictor(x):
        return 1 if dotProduct(weights, featureExtractor(x)) >= 0 else -1

    # BEGIN_YOUR_CODE (our solution is 13 lines of code, but don't worry if you deviate from this)
    for epoch in range(numEpochs):
        for x,y in trainExamples:
            features_dict = featureExtractor(x)
            hinge_loss = dotProduct(features_dict, weights) * y

            if hinge_loss < 1 :
                for feature in features_dict:
                    if feature not in weights:
                        weights[feature] = 0


                    gradient = - features_dict[feature]*y
                    weights[feature] = weights[feature] - eta*(gradient)
                    

    #Evaluate on training and validation sets
    # print("Weights:", weights)
    print("Train loss: ", evaluatePredictor(trainExamples, predictor) )
    print("Validation loss: ", evaluatePredictor(validationExamples, predictor) )
        
    print("\n")

    # END_YOUR_CODE
    return weights


############################################################
# Problem 3c: generate test case


def generateDataset(numExamples: int, weights: WeightVector) -> List[Example]:
    '''
    Return a set of examples (phi(x), y) randomly which are classified
      correctly by |weights|.
    '''
    random.seed(42)

    # Return a single example (phi(x), y).
    # phi(x) should be a dict whose keys are a subset of the keys in weights
    # and values can be anything (randomize!) with a score for the given weight vector.
    # note that there is intentionally flexibility in how you define phi.
    # y should be 1 or -1 as classified by the weight vector.
    # y should be 1 if the score is precisely 0.

    # Note that the weight vector can be arbitrary during testing.
    def generateExample() -> Tuple[Dict[str, int], int]:
        # BEGIN_YOUR_CODE (our solution is 3 lines of code, but don't worry if you deviate from this)
        keys = random.sample(list(weights.keys()), k=random.randint(1, len(list(weights.keys()))))
        phi = {key: random.randint(-1, 1) for key in keys}  # Randomize values for phi
        score = sum(weights[key] * phi[key] for key in keys)  # Calculate score based on weights
        y = 1 if score == 0 else (1 if score > 0 else -1)  # Classify based on the score
        return phi, y

    return [generateExample() for _ in range(numExamples)]


############################################################
# Problem 3d: character features


def extractCharacterFeatures(n: int) -> Callable[[str], FeatureVector]:
    '''
    Return a function that takes a string |x| and returns a sparse feature
    vector consisting of all n-grams of |x| without spaces mapped to their n-gram counts.
    EXAMPLE: (n = 3) "I like tacos" --> {'Ili': 1, 'lik': 1, 'ike': 1, ...
    You may assume that 1 <= n <= len(x).
    '''
    def extract(x: str) -> Dict[str, int]:
        # BEGIN_YOUR_CODE (our solution is 6 lines of code, but don't worry if you deviate from this)
        x = x.replace(" ", "")  # Remove spaces
        ngrams = {}
        for i in range(len(x) - n + 1):
            ngram = x[i:i + n]  # Get the n-gram
            if ngram in ngrams:
                ngrams[ngram] += 1  # Increment the count if it already exists
            else:
                ngrams[ngram] = 1   # Initialize the count
        
        return ngrams
        # END_YOUR_CODE

    return extract


############################################################
# Problem 3e:


def testValuesOfN(n: int):
    '''
    Use this code to test different values of n for extractCharacterFeatures
    This code is exclusively for testing.
    Your full written solution for this problem must be in sentiment.pdf.
    '''
    trainExamples = readExamples('polarity.train')
    validationExamples = readExamples('polarity.dev')
    featureExtractor = extractCharacterFeatures(n)
    weights = learnPredictor(trainExamples,
                             validationExamples,
                             featureExtractor,
                             numEpochs=20,
                             eta=0.01)
    outputWeights(weights, 'weights')
    outputErrorAnalysis(validationExamples, featureExtractor, weights,
                        'error-analysis')  # Use this to debug
    trainError = evaluatePredictor(
        trainExamples, lambda x:
        (1 if dotProduct(featureExtractor(x), weights) >= 0 else -1))
    validationError = evaluatePredictor(
        validationExamples, lambda x:
        (1 if dotProduct(featureExtractor(x), weights) >= 0 else -1))
    print(("Official: train error = %s, validation error = %s" %
           (trainError, validationError)))


############################################################
# Problem 5: k-means
############################################################




def kmeans(examples: List[Dict[str, float]], K: int,
           maxEpochs: int) -> Tuple[List, List, float]:
    '''
    Perform K-means clustering on |examples|, where each example is a sparse feature vector.

    examples: list of examples, each example is a string-to-float dict representing a sparse vector.
    K: number of desired clusters. Assume that 0 < K <= |examples|.
    maxEpochs: maximum number of epochs to run (you should terminate early if the algorithm converges).
    Return: (length K list of cluster centroids,
            list of assignments (i.e. if examples[i] belongs to centers[j], then assignments[i] = j),
            final reconstruction loss)
    '''
    # BEGIN_YOUR_CODE (our solution is 28 lines of code, but don't worry if you deviate from this)

    def get_distance(d1, d2):
        """
        @param dict d1: a feature vector represented by a mapping from a feature (string) to a weight (float).
        @param dict d2: same as d1
        @return float: the L2 norm between d1 and d2
        """
        return sum((d1.get(f, 0) - v)**2 for f, v in list(d2.items()))
      
    centers = random.sample(examples, K)
    num_examples = len(examples)
    
    for epoch in range(maxEpochs):
        allocated_center = [None for x in range(num_examples)]
        
        ### allocate centers
        
        for i in range(num_examples):
            best_distance = float("inf")
            
            for j in range(K):
                distance = get_distance(centers[j], examples[i])
                
                if distance < best_distance:
                    best_distance = distance
                    allocated_center[i] = j

        ### calculate new centers
        
        new_centers = [DefaultDict(int) for x in range(K)]
        count = [DefaultDict(int) for x in range(K)] #each feature vector in a cluster as its own count
        
        for i in range(num_examples):
            center = new_centers[allocated_center[i]]
            
            for a ,b in list(examples[i].items()):
                new_count = count[allocated_center[i]][a] + 1 #new count for a particular feature vector in a cluster
                center[a] = (center[a]*count[allocated_center[i]][a] + b)/new_count
                count[allocated_center[i]][a] = new_count
        
        if centers == new_centers:
            break
        else:
            centers = new_centers
            
    def reconstruction_loss():
        loss = 0
        for i in range(num_examples):
            loss += get_distance(centers[allocated_center[i]], examples[i])
        return loss
    
    return (
        centers,
        allocated_center,
        reconstruction_loss()
        )
