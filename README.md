# Introduction  
This is the 1st project of the course "Software Development for Algorithmic Problems". In this 
project, we achieved the following:  
1. implemented 2 differrent approaches to tackle the approximate nearest neighbour search 
problem: **LSH** and **Hypercube Randomized Projection** 
2. implemented the improved version of the well-known clustering algorithm k-Means, which is 
called **k-Medians++**  

The dataset used in the 2 tasks above was [MNIST](http://yann.lecun.com/exdb/mnist/). Each 
handwritten digit image has a resolution of 28x28 pixels. Consequently, we store each image 
as a "flattened" vector of size 784 (28 x 28 = 784). To calulate the distance between 2 points 
in our datasets we used the manhattan distance

# Nearest Neighbour Search  
Both methods mentioned above (LSH and Hypercube) work in a similar manner. The following 
sequence of steps happens before the actual search process takes place:  
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
Using the same dataset file as input, the goal of this program is to "group" as accurately as 
possible the input datapoints into clusters. Ideally, the clusters produced by the program 
should only contain images of the same handwritten digit (default numbers of clusters is 10) .  
The (iterative) algorithm selects its initial centroids using an improved initialization 
technique called [initialization++](https://en.wikipedia.org/wiki/K-means%2B%2B#Improved_initialization_algorithm), assigns points to their closest centroid using one of Lloyd's or LSH or 
Hypercube assignment methods and uses the median update rule to update the centroids.  
The algorithm stops, when the observed change in cluster assignments is relatively small. For 
this purpose, the k-medians objective function (l1 norm) is calculated after each iteration.


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

Finally, for **clustering**, run the following commands:  
```
$ cd src/cluster
$ make
$ ./cluster -i ../../datasets/train-images-idx3-ubyte -c ../../include/cluster/cluster.conf
            -o <output_file> --complete <optional> -m <method: Classic or LSH or Hypercube>
```  

The formatted output will be written to the output file specified by the user.
This is how it looks like:  
```
Algorithm: Lloyds OR Range Search LSH OR Range Search Hypercube
CLUSTER-1 {size: <int>, centroid: array with the centroid's components}
...
CLUSTER-Κ {size: <int>, centroid: array with the centroid's components}
clustering_time: <double> //in seconds
Silhouette: [s1,...,si,...,sΚ, stotal]
// si=average s(p) of points in cluster i, stotal=average s(p) of points in dataset
// Optionally with command line parameter –complete 
CLUSTER-1 {centroid, image_numberA, ..., image_numberX}
...
CLUSTER-Κ {centroid, image_numberR, ..., image_numberZ}
```
