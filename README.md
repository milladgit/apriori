# Apriori algorithm in C++

This algorithm is written in C++. It will read the input file (text file containing all the transactions in the DB) and will output the results to a file (provided by user as the command argument).

### Build:
	make

### Run:
	./apriori <minsup_count> <K> <input_file> <output_file>

	minsup_count: Minimum support count (minsup_percentage * number of transactions)
	K: frequent K-itemset
	input_file: transaction dataset 
	output_file: final result file

Note: all the arguments are required.



## Format of the input:
The input file should have the following input. Each line represents a transaction, which each of the itemsets in the transactions are separated by a "space". Itemsets can be composed of any character set of strings.

*Hint:* If your format changes from what was discribed above, you can easily change only the "readFile" function the file.


# Speed and Efficiency
In this code, speed and efficiency was the first goal. The algorithm is designed to provide the results as fast as possible considering exploiting only one code. 

## Multi-threaded version
Work-in-progress: Using OpenMP features

