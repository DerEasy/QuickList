# QuickList
C++ implementation of a Doubly Linked List, but with drastically improved search, insertion and deletion by index speed.

### Advantages:
- Can hold huge amounts of objects (even millions) and retain efficiency when searching, inserting or deleting by index
- Also suitable for small lists, albeit requiring more memory percentually
- Extra memory usage in percent decreases with the amount of objects in the list: 

  10% up to 449 objects
  
  5% up to 1249 objects
  
  3,33% up to 2449 objects
  
  0.04464% at 10 million objects

### Disadvantages:
- Slightly higher memory usage
- Degrades to a normal list when size is < 10
- Keeping the efficiency high requires rebuilding the JumpList when the QuickList exceeds/subceeds its current critical size, which is a costly one-time operation, but pays off by its results

## How it works
### JumpList
The JumpList is a seperate list that is created as a member of a QuickList.
It holds pointers that point to evenly spaced nodes in the QuickList.

Initially the distance between these JumpPointers is 10. That means that every tenth node in the QuickList has a JumpPointer that is pointing to it excluding the first node.
Thus, when searching, inserting or deleting a node by index, the JumpList is used to jump to a position in the list that is smaller or equal to the index that is requested, iterating the QuickList from that node on.

Example:
- Scenario: Searching for the 242nd node in a QuickList with 400 nodes
- JumpList has 400 / distance(10) = 40 JumpPointers in it
- Iterating the JumpList from tail to its 24th node
- Jumping to the 240th node in the QuickList using the JumpPointer
- Iterating the QuickList two times to the right to arrive at the 242nd node
- Returning that node by merely taking 18 total steps to iterate to its position

### Rebuilding the JumpList and critical size
The distance between JumpPointers increases everytime the QuickList hits its current upper critical size, that is the size at which point using larger spaced JumpPointers becomes more efficient. This is called 'rebuilding the JumpList'; the distance is always a multiple of 10.

Rebuilding the JumpList therefore takes place with every multiple of 10 on the x-axis in the function 2x²+20x with x being the JumpPointer distance and y being the amount of nodes in the QuickList. Exactly the opposite thing happens when the size of the QuickList subceeds the lower critical size; the distance between the JumpPointers is decreased by 10. To avoid unnecessarily rebuilding the JumpList when you are adding and removing nodes constantly right at the critical size, the lower critical size is shifted by -50, therefore taking the function 2x²-20x-50, which gives you a buffer of 50 nodes to keep up the overall performance.

![alt text](https://github.com/DerEasy/QuickList/blob/main/images/QuickList%20critical%20size%20graph.png)

Here you can see the critical sizes at which the JumpList will be rebuilt to the next/previous JumpPointer distance. This table also shows some examples for the first few JumpPointer distances. Rebuilding from 10 to 20 is the only distance increase that is much faster than all others, as it suffices to just delete every second node, instead of clearing the JumpList, iterating the entire QuickList and setting the JumpPointers accordingly.
|JumpPointer Distance|Lower critical size|Upper critical size|
|--------------------|-------------------|-------------------|
|10                  |None               |400                |
|20                  |350                |1200               |
|30                  |1150               |2400               |
|40                  |2350               |4000               |
|50                  |3950               |6000               |
|60                  |5950               |8400               |
|70                  |8350               |11200              |

Due to the initial and lowest JumpPointer distance being 10, the QuickList is no different from a normal doubly linked list until a 10th node has been appended to it.
The extra memory usage of the JumpList in percent is calculated with 1/10x with x being the JumpPointer distance thus:
|JumpPointer Distance|Extra memory usage|Maximum size of QuickList|Maximum amount of JumpPointers|
|--------------------|------------------|-------------------------|------------------------------|
|10                  |10%               |399                      |39                            |
|20                  |5%                |1199                     |59                            |
|30                  |3.33%             |2399                     |79                            |
|40                  |2.5%              |3999                     |99                            |
|50                  |2%                |5999                     |119                           |
|60                  |1.66%             |8399                     |139                           |
|70                  |1.42%             |11199                    |159                           |




