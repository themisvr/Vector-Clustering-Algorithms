# Introduction  
This is the 1st project of the course "Software Development for Algorithmic Problems". In this 
project, we achieved the following:  
1. implemented 2 differrent approaches to tackle the nearest neighbour search problem: **LSH** 
and **Hypercube Randomized Projection** 
2. implemented the improved version of the well-known clustering algorithm k-Means, which is 
called **k-Medians++**
The dataset used in the 2 tasks above was [MNIST](http://yann.lecun.com/exdb/mnist/)

# Nearest Neighbour Search  
Both methods mentioned above work in a similar manner. The following sequence of steps happens 
before the actual search process takes place:  
1. program reads in the input dataset (or training set), which in our case consists of 60.000 
images of handwritten digits (0 - 9). 
2. then, the program builds the actual data structures that will be used in the search process.
All input dataset points are stored in these data structures
3. next, the program reads in the query set (or test set)
4. search starts: for each point in the query set find:
  1. its _N_ nearest neighbours approximately (Approximate k-NN)
  2. its _N_ nearest neighbours using brute-force search (Exact NN)
  3. its nearest neighbours approximately that lie inside a circle of radius R
Where these 2 methods differ, is how each one builds its appropriate data structures and chooses to store (hashing) the input dataset. The whole purpose of these 2 methods is to deliver an 
efficient -but approximate- type search that significantly reduces search time compared to 
Exact-NN, but it also produces high accuracy results


# Clustering


# Execution



# Vector-Clustering-Algorithms
In this application we are called to implement clustering algorithms for vectors using Locality Sensitive Hashing. Also, the goal is to reduce the dimensionality with random projection on Hypercube.
