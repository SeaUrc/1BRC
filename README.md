# Kinda 1BRC

### CSV Generation 

generate the csv file from generatecsv.py. Some guarantees:
- max 43 cities
- carriage return
- always contains one decimal place


### Spec
I'm using a M1 Macbook Pro with 8GB of memory.

### Deps
I use google's benchmarking library. I used abseil's flat hash map at one point but the best ver. doesn't use it.

### Strat
Memory map the file and set concurrent readers. Within a given line, compute a rolling hash and index into a per-thread array. I've adjusted the size of the array such that theres no collision (so no checks are used).
Then aggregate the results into a map and print out. Advising the pointers didn't seem to make a difference. Nor did untieing std in and out. Setting the quality of choice of each thread seemed to help a bit.

### Results
For 1 billion row and using time,
```
13.69s user 16.41s system 180% cpu 16.704 total
```

For 100 million rows and using time,
```
1.77s user 0.33s system 668% cpu 0.314 total
```

Some interesting things to think about, 
- the 1 billion rows is ~53x slower than the 100 million rows. Perhaps this has something to do with the fact that 100 mil. csv <8GB while 1bil csv is >8GB. The OS could have somehow cached the file somewhere before.
- the 1 billion rows had 180% cpu while 100 mil rows has 668% cpu. In a similar vein, threads could be blocked much more by I/O.