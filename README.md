# QuickList [WIP]
C++ implementation of a Doubly Linked List, but with drastically improved search, insertion and deletion by index speed. Time Complexity: O(√n). Benchmarks at the bottom.

# The QuickList is currently being reworked to guarantee a worst case time complexity of O(sqrt(n)). Information below may be outdated, including graphs.

### Advantages:
- Can hold huge amounts of nodes (even millions) and retain remarkable efficiency when searching, inserting or deleting by index (explained below)
- Also suitable for small lists, albeit requiring more memory percentually
- Extra memory usage in percent decreases with the amount of nodes in the list
- Works exactly the same as a normal List on the surface/for the programmer, although its inner workings make for much better performance, thus it includes all uses and advantages of Lists, but with severely weakened disadvantages
- Generally a good replacement for normal Lists when not coding for embedded systems/a little extra memory is not a problem etc
- Easy to use

### Disadvantages:
- Slightly higher memory usage (explained below)
- Degrades to a normal list when size is < 10
- Keeping the efficiency high requires rebuilding the JumpList when the QuickList exceeds/subceeds its current critical size, which is a costly one-time operation, but pays off by its results

#### Hint:
Due to the nature of data structures in general, operations by value cannot be sped up, as you would always need to iterate every node and check its value. This cannot be mitigated, so the QuickList has identical performance compared to a normal List regarding these operations.

## How it works
### JumpList
The JumpList is a seperate list that is created as a member of a QuickList.
It holds pointers that point to evenly spaced nodes in the QuickList.

Initially the distance between these JumpPointers is 10. That means that every tenth node in the QuickList has a JumpPointer that is pointing to it excluding the first node.
Thus, when searching, inserting or deleting a node by index, the JumpList is used to jump to a position in the list that is smaller or equal to the index that is requested, iterating the QuickList from that node on.

#### Example:
- Scenario: Searching for the 242nd node in a QuickList with 400 nodes
- JumpList has 400 / distance(10) = 40 JumpPointers in it
- Iterating the JumpList from tail to its 24th node
- Jumping to the 240th node in the QuickList using the JumpPointer
- Iterating the QuickList two times to the right to arrive at the 242nd node
- Returning that node by merely taking 18 total steps to iterate to its position instead of 158

This speedup gets more dramatic the bigger the QuickList.

### Rebuilding the JumpList and critical size
The distance between JumpPointers increases everytime the QuickList hits its current upper critical size, that is the size at which point using larger spaced JumpPointers becomes more efficient. This is called 'rebuilding the JumpList'; the distance is always a multiple of 10.

Rebuilding the JumpList therefore takes place with every multiple of 10 on the x-axis in the function 2x²+20x with x being the JumpPointer distance and y being the amount of nodes in the QuickList. Exactly the opposite thing happens when the size of the QuickList subceeds the lower critical size; the distance between the JumpPointers is decreased by 10. To avoid unnecessarily rebuilding the JumpList when you are adding and removing nodes constantly right at the critical size, the lower critical size is shifted by -50, therefore taking the function 2x²-20x-50, which gives you a buffer of 50 nodes to keep up the overall performance.

Rebuilding the JumpList is a fully automatic and dynamic process, that shall not be interfered with, thus there is no functionality to alter the QuickList's/JumpList's logic.

![alt text](https://github.com/DerEasy/QuickList/blob/main/images/QuickList%20critical%20size%20graph.png)

Here you can see the critical sizes at which the JumpList will be rebuilt to the next/previous JumpPointer distance. This table also shows some examples for the first few JumpPointer distances.
|JumpPointer Distance|Lower critical size|Upper critical size|
|--------------------|-------------------|-------------------|
|10                  |None               |400                |
|20                  |350                |1200               |
|30                  |1150               |2400               |
|40                  |2350               |4000               |
|50                  |3950               |6000               |
|60                  |5950               |8400               |
|70                  |8350               |11200              |

### Memory usage
Due to the initial and lowest JumpPointer distance being 10, the QuickList is no different from a normal doubly linked list until a 10th node has been appended to it.
The extra memory usage because of the JumpList in percent is calculated with 1/x with x being the JumpPointer distance thus:
|JumpPointer Distance|Extra memory usage|Maximum size of QuickList|Maximum amount of JumpPointers|
|--------------------|------------------|-------------------------|------------------------------|
|10                  |10%               |399                      |39                            |
|20                  |5%                |1199                     |59                            |
|30                  |3.33%             |2399                     |79                            |
|40                  |2.5%              |3999                     |99                            |
|50                  |2%                |5999                     |119                           |
|60                  |1.66%             |8399                     |139                           |
|70                  |1.42%             |11199                    |159                           |

## Benchmark
### Comparison: 10000 single search operations (exact middle)
|Amount of nodes|std::list|QuickList|Time difference|Speedup|
|---------------|---------|---------|---------------|-------|
|1,000,000      |17454ms  |98ms     |-99.4%         |+17710%|
|100,000        |1717ms   |35ms     |-98%           |+4805% |
|10,000         |177ms    |11ms     |-93.8%         |+1509% |







