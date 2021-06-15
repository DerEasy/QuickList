# QuickList
C++ implementation of a Doubly Linked List, but with drastically improved search, insertion and deletion by index speed.

Advantages:
- Can hold huge amounts of objects (even millions) and retain efficiency when searching, inserting or deleting by index
- Also suitable for small lists, albeit requiring more memory percentually
- Extra memory usage in percent decreases with the amount of objects in the list: 

  10% up to 449 objects
  
  5% up to 1249 objects
  
  3,33% up to 2449 objects
  
  0.04464% at 10 million objects

Disadvantages:
- Slightly higher memory usage
- Degrades to a normal list when size is < 10
- Keeping the efficiency high requires rebuilding the JumpList when the QuickList exceeds/subceeds its current critical size, which is a costly one-time operation, but pays off by its results
