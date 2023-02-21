# Parallel-processing-using-the-Map-Reduce-paradigm

<p align="justify">
This is the implementation of a parallel program in Pthreads for finding numbers greater than 0 that are perfect powers from a set of files and counting unique values for each exponent.
</p>
<p align="justify">
To parallelize the processing of input documents, the Map-Reduce model is used. The input files are divided (dynamically) as evenly as possible among a set of threads that will parse and check for numbers greater than 0 that are perfect powers (Map operation), resulting in partial lists for each exponent (e.g., lists for perfect squares, lists for perfect cubes, etc.). The next step is to combine the partial lists (Reduce operation), resulting in aggregated lists for each exponent. For each such list, unique values are counted in parallel, and the results are then written to output files. <br>
</p>
  
### Map-Reduce Paradigm

<p align="justify">
A Map-Reduce model similar to the one used by Google for processing large sets of documents in distributed systems is used. This article presents the Map-Reduce model used by Google and some of its applications.
</p>

<p align="justify">
Map-Reduce is a parallel programming model for processing vast amounts of data using hundreds or thousands of processors. In most cases, Map-Reduce is used in a distributed context and is, in fact, a programming model that can be adapted to both situations. The model allows for automatic parallelization and distribution of tasks. The Map-Reduce paradigm is based on the existence of two functions that give it its name: Map and Reduce. The Map function takes as input a function f and a list of elements and returns a new list of elements resulting from applying function f to each element in the original list. The Reduce function combines the previously obtained results. <br>
</p>
  
### Map Operations

<p align="justify">
Starting from the list of documents to be processed, each Mapper will end up processing some documents. The allocation of documents to Mapper threads is done dynamically as the documents are processed. Each Mapper will perform the following actions for each file it is responsible for:
</p>

<p align="justify">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* open the file and read it line by line. <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* for each integer read, check if it is a perfect power of 2, 3, etc., up to the maximum exponent (given by the number of Reducer threads). <br>
&nbsp;&nbsp;&nbsp;&nbsp;* each value greater than 0 that is a perfect power with an exponent E is saved to a partial list (if a value is a perfect power for multiple exponents E, it will be placed in multiple partial lists). <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* when a file has been processed, the Mapper closes. <br>
</p>

### Reduce Operations

<p align="justify">
A Reducer thread will be responsible for aggregating and counting perfect power values for a single exponent (e.g., one Reducer will handle perfect squares, another perfect cubes, etc.). Thus, given the results from the Map operation, a Reducer will perform the following actions:
</p>

<p align="justify">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* combines the partial lists for the exponent E it is responsible for into an aggregated list (combining step) <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* counts the unique values in the aggregated list and write the result to a file (processing step). <br>
</p>
