# Introduction  
This is the 1st project of the course "Software Development for Algorithmic Problems". In this 
project, we achieved the following:  
1. implemented 2 differrent approaches to tackle the approximate nearest neighbour search 
problem: **LSH** and **Hypercube Randomized Projection** 
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
  2. its _N_ nearest neighbours using brute-force search (Exact k-NN)
  3. its nearest neighbours approximately that lie inside a circle of radius _R_

Where these 2 methods differ, is how each one builds its appropriate data structures and chooses to store (hashing) the input dataset.  
In general, The whole purpose of these 2 methods is to deliver an efficient -but approximate- 
type search that significantly reduces search time compared to Exact k-NN, but it also produces 
high accuracy results


# Clustering


# Execution
For the **LSH** method, run the following commands:  
```
$ cd src/lsh  
$ make  
$ ./lsh -d ../../datasets/train-images-idx3-ubyte -q ../../datasets/t10k-images-idx3-ubyte 
        -k <num_hash_functions> -L <num_hash_tables> -o <output_file> 
        -N <num_of_nearest_neighbours> -R <radius>
```
For the **Hypercube** method, run the following commands:  
```
$ cd src/cube  
$ make  
$ ./cube -d ../../datasets/train-images-idx3-ubyte -q ../../datasets/t10k-images-idx3-ubyte 
         -k <projection_dimension> -M <max_candidates> -probes <max_probes> -o <output_file> 
         -N <num_of_nearest_neighbours> -R <radius>
```
Both methods can also run without explicit command line arguments i.e by simply running 
`$ ./lsh` or `$ ./cube` after of course navigating to the appropriate directory.
In this case, default argument values will be used.  
The formatted output will be written to the output file specified by the user.
This is how it looks like:  
```
Query: image_number_in_query_set
Nearest neighbor-1: image_number_in_dataset
distanceLSH: <int> [or distanceHypercube respectively]
distanceTrue: <int>
...
Nearest neighbor-N: image_number_in_dataset
distanceLSH: <int> [or distanceHypercube respectively]
distanceTrue: <int>
tLSH: <double>  [or tHypercube respectively]
tTrue: <double>
R-near neighbors:
image_number_A
image_number_B
. . .
image_number_Z
```

# Vector-Clustering-Algorithms
In this application we are called to implement clustering algorithms for vectors using Locality Sensitive Hashing. Also, the goal is to reduce the dimensionality with random projection on Hypercube.
