#include <iostream>
#include <cmath>
#include "jumplist.cpp"

template <typename T>
class QuickList : public BaseList<T> {
public:
    /**
     * The TrailingPointer allows for constant time sequential operations. Otherwise, it is used
     * if it is more efficient to iterate the list from its Trailing index than it is to iterate
     * from head or tail. It does this by saving the search result of the last search operation.
     */
    typedef struct TrailingPointer {
        int index;
        Node<Node<T>*>* jumpPointer;
        Node<T>* node;
    } TrailingPointer;

    /**
     * Is returned by search operations. Incorporates the searched node and the jumpPointer
     * it has been accessed from.
     */
    typedef struct searchResult {
        Node<T>* node;
        Node<Node<T>*>* jumpPointer;
    } searchResult;

    typedef struct searchCheck {
        bool done = false;
        searchResult r {nullptr, nullptr};
    } searchCheck;

    //JumpList provides the JumpPointers that point to individual nodes in the QuickList for fast access
    //This is the core functionality of a QuickList
    JumpList<T> jumpList;

    //TrailingPointer keeps track of the last used node and jumpPointer
    //to speed up (semi-)sequential rw operations
    TrailingPointer trailingPointer = {-1, nullptr, nullptr};

    //Distance between individual JumpPointers. Initial: 10
    //Always a multiple of 10, never below 10
    //Example: JumpPointers point to QuickList index 9, 19, 29, 39, 49,...
    int distance = 10;

    QuickList() {
        jumpList.getHead()->setData(this->getHead());
        jumpList.getTail()->setData(this->getTail());
    }

    /**
     * Increases the size of the QuickList and automatically rebuilds the JumpList or adds another JumpPointer if necessary
     */
    void incSize() override {
        this->size++;
        //Ensures that no duplicate JumpPointers get appended
        bool rebuilt = rebuildJumpList();

        if (!rebuilt && getsJumpPointer())
            addJumpPointer();
    }

    /**
     * Decreases the size of the QuickList and automatically rebuilds the JumpList or removes the last JumpPointer if necessary
     */
    void decSize() override {
        this->size--;
        rebuildJumpList();

        if (losesJumpPointer())
            removeJumpPointer();
    }

    /**
     * @return True if the QuickList has reached a size dividable by distance at which point it needs another JumpPointer
     */
    bool getsJumpPointer() {
        return this->getSize() != 0 &&
        this->getSize() % distance == 0;
    }

    /**
     * @return True if the QuickList subceeds a size dividable by distance at which point it needs to remove the last JumpPointer
     */
    bool losesJumpPointer() {return this->getSize() % distance == distance - 1;}

    /**
     * Appends a JumpPointer to the JumpList
     */
    void addJumpPointer() {jumpList.append(this->getLastNode());}

    /**
     * Removes the last JumpPointer
     */
    void removeJumpPointer() {jumpList.removeLast();}

    void concat(QuickList<T>* quickList) {
        Node<T>* node = quickList->getFirstNode();
        while (node != quickList->getTail()) {
            this->append(node->getData());
            node = node->getNextNode();
        }
    }

    void concat(T array[], int arraySize) {
        for (int i = 0; i < arraySize; i++)
            this->append(array[i]);
    }

    /**
     * Calculates the distance by using the QuickList size
     * @return distance value as a multiple of 10
     */
    int calcDistance() {
        int x = (int) (-5 + sqrt(pow(5, 2) + this->getSize()));
        x -= x % 10;
        return x + 10;
    }

    /**
     * Calculates the upper critical size of the QuickList by using the distance
     * @return upper critical size value
     */
    int upperCritical() {return pow(distance, 2) + 10 * distance;}

    /**
     * Calculates the lower critical size of the QuickList by using the distance
     * @return lower critical size value
     */
    int lowerCritical() {return pow(distance, 2) - 10 * distance - 50;}

    /**
     * Attempts to rebuild the JumpList. Succeeds if upper or lower critical size has been reached.
     * @return True if JumpList has been rebuilt
     */
    bool rebuildJumpList() {
        if (this->getSize() >= upperCritical() || this->getSize() <= lowerCritical()) {
            jumpList.clear();
            distance = calcDistance();
            Node<T>* node = this->getFirstNode();

            int index = 1;
            while (this->hasNext(node)) {
                if (index % distance == 0)
                    jumpList.append(node);
                node = node->getNextNode();
                index++;
            }
            return true;
        }
        return false;
    }

    /**
     * Updates the values of the TrailingPointer.
     * The jumpPointer
     *must be a JumpPointer that points to or below the index.
     * @param index Last used index
     * @param node Last used node
     * @param jumpPointer
     *Last used jumpPointer
     *
     */
    void setTrailingPointer(int index, Node<T>* node, Node<Node<T>*>* jumpPointer) {
        trailingPointer.index = index;
        trailingPointer.node = node;
        trailingPointer.jumpPointer= jumpPointer;
    }

    /**
     * Attempts to invalidate the TrailingPointer. Used when the index/node/jumpPointer
     *becomes
     * unavailable due to removal etc. The TrailingPointer is hence only invalidated by this
     * method if its saved index matches the parameter index.
     * @param index Determines if the TrailingPointer needs to be invalidated or not
     * @return True if invalidated
     */
    bool invalidateTrailingPointer(int index) {
        if (trailingPointer.index == index) {
            trailingPointer.index = -1;
            trailingPointer.node = nullptr;
            trailingPointer.jumpPointer= nullptr;
            return true;
        }
        return false;
    }

    void forceInvalidateTrailingPointer() {
        trailingPointer.index = -1;
        trailingPointer.node = nullptr;
        trailingPointer.jumpPointer= nullptr;
    }

    /**
     * Checks if using the TrailingPointer is viable (the most efficient solution) or not
     * @param index The searched-for index
     * @return True if search with TrailingPointer is to be used
     */
    bool trailingPointerViable(int index) {
        return
        trailingPointer.index != -1 &&
        abs(0 - index) > abs(trailingPointer.index - index) &&
        abs(this->getMaxIndex() - index) > abs(trailingPointer.index - index);
    }

    /**
     * @return The index of the pointed-to node by the TrailingPointer JumpPointer
     */
    int getTrailingJumpPointerIndex() {
        if (!indexHasJumpPointer(trailingPointer.index))
            return ((trailingPointer.index - trailingPointer.index % distance) / distance) - 1;
        else
            return (((trailingPointer.index + 1) - (trailingPointer.index + 1) % distance) / distance) - 1;
    }

    /**
     * @param index The searched-for index
     * @return JumpList(!) offset for the TrailingPointer JumpPointer
     */
    int trailingJumpPointerOffset(int index) {return (index + 1) / distance - (trailingPointer.index + 1) / distance;}

    /**
     * Scenario: JumpPointers point to indices 119, 139, 159, and you want to get the nearest index of the node
     * that is pointed to by a JumpPointer. Input index is 155. This method will return 139 then (It is always
     * the previous node that is returned, never the next, even if the next one is nearer, like in this case).
     * If the input index is equal to the index of a node pointed to by a JumpPointer, that same index is
     * returned, e.g. 139 if your input is 139 in the above example.
     * @param index
     * @return The index of the node that is pointed to by a JumpPointer
     */
    int getJumpIndex(int index) {
        if (indexHasJumpPointer(index))
            return index;
        else
            return (index - index % distance) - 1;
    }

    /**
     * This method returns its result only by using arithmetics with the
     * distance variable. This means that it may return true even if the
     * index is out of range.
     * @param index
     * @return True if index has a JumpPointer
     */
    bool indexHasJumpPointer(int index) {return (index + 1) % distance == 0;}

    /**
     * @param index
     * @return True if the JumpPointer should be traversed backwards for better efficiency
     */
    bool iterateJumpPointerBackwards(int index) {
        int jumpIndex = getJumpIndex(index);
        int nextJumpIndex = jumpIndex + distance;
        return index - jumpIndex > nextJumpIndex - index;
    }

    typedef struct TrailingSearchNode {
        Node<T>* node;
        int indexOffset;
    } TrailingSearchNode;

    TrailingSearchNode bestTrailingPointer(int index) {
        //0 is prevJPtr
        //1 is trailingPtr
        //2 is nextJPtr
        int difference[3];
        int prevJPtrIndex = getJumpIndex(index);
        int nextJPtrIndex = prevJPtrIndex + distance;

        difference[0] = index - prevJPtrIndex;
        difference[1] = index - trailingPointer.index;
        difference[2] = index - nextJPtrIndex;

        TrailingSearchNode t;

        if (abs(difference[0]) < abs(difference[1])) {
            t.node = trailingPointer.jumpPointer->getData();
            t.indexOffset = difference[0];
        } else if (nextJPtrIndex < this->getMaxIndex() && abs(difference[2]) < abs(difference[1])) {
            t.node = trailingPointer.jumpPointer->getNextNode()->getData();
            t.indexOffset = difference[2];
        } else {
            t.node = trailingPointer.node;
            t.indexOffset = difference[1];
        }
        return t;
    }

    searchResult trailingSearch(int index) {
        searchResult r;
        r.jumpPointer = trailingPointer.jumpPointer;
        r.node = trailingPointer.node;

        //Amount of steps that the JumpPointer has to jump
        int jPtrOffset = trailingJumpPointerOffset(index);

        //If the JumpPointer is in range, do not modify the values given by the TrailingPointer
        if (jPtrOffset == 0) {
            TrailingSearchNode t = bestTrailingPointer(index);

            //Shift the node indexOffset times in the given direction
            if (t.indexOffset > 0)
                for (int i = 0; i < t.indexOffset; i++)
                    t.node = t.node->getNextNode();
            else if (t.indexOffset < 0)
                for (int i = 0; i > t.indexOffset; i--)
                    t.node = t.node->getPrevNode();

            r.node = t.node;
            setTrailingPointer(index, r.node, r.jumpPointer);
            return r;

        } else {
            if (jPtrOffset > 0)
                for (int i = 0; i < jPtrOffset; i++)
                    r.jumpPointer= r.jumpPointer->getNextNode();
            else
                for (int i = jPtrOffset; i < 0; i++)
                    r.jumpPointer= r.jumpPointer->getPrevNode();

            if (indexHasJumpPointer(index)) {
                r.node = r.jumpPointer->getData();
                setTrailingPointer(index, r.node, r.jumpPointer);
                return r;
            }

            bool iterateJPtrBackwards = iterateJumpPointerBackwards(index);
            if (iterateJPtrBackwards)
                r.jumpPointer= r.jumpPointer->getNextNode();

            r.node = r.jumpPointer->getData();

            if (iterateJPtrBackwards) {
                for (int i = 0; i <= (distance - index % distance) - 2; i++)
                    r.node = r.node->getPrevNode();
                setTrailingPointer(index, r.node, r.jumpPointer->getPrevNode());
            } else {
                for (int i = 0; i <= index % distance; i++)
                    r.node = r.node->getNextNode();
                setTrailingPointer(index, r.node, r.jumpPointer);
            }

            return r;
        }
    }

    searchResult nonTrailingSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);
        bool iterateJPtrBackwards = iterateJumpPointerBackwards(index);

        //Decide starting point of search
        forward ?
        r.jumpPointer= jumpList.getHead():
        r.jumpPointer= jumpList.getTail();

        //Check if jumpList should be iterated or if search should start at the head
        //Depending on where search starts, the jumpList has to iterate forwards or backwards
        if (forward) {
            for (int i = 0; jumpList.hasNext(r.jumpPointer); i++, r.jumpPointer= r.jumpPointer->getNextNode())
                if (i == index / distance)
                    break;
        } else {
            for (int i = -1; jumpList.hasPrev(r.jumpPointer); i++, r.jumpPointer= r.jumpPointer->getPrevNode())
                if (i == jumpList.getSize() - (index / distance))
                    break;
        }

        if (indexHasJumpPointer(index)) {
            r.jumpPointer= r.jumpPointer->getNextNode();
            r.node = r.jumpPointer->getData();
            setTrailingPointer(index, r.node, r.jumpPointer);
            return r;
        }
        if (iterateJPtrBackwards)
            r.jumpPointer= r.jumpPointer->getNextNode();

        //Sets node to the data of the determined jumpPointer
        r.node = r.jumpPointer->getData();

        //Iterate from pointed-to node to desired node
        if (iterateJPtrBackwards) {
            for (int i = 0; i <= (distance - index % distance) - 2; i++)
                r.node = r.node->getPrevNode();
            setTrailingPointer(index, r.node, r.jumpPointer->getPrevNode());
        } else {
            for (int i = 0; i <= index % distance; i++)
                r.node = r.node->getNextNode();
            setTrailingPointer(index, r.node, r.jumpPointer);
        }

        return r;
    }

    /**
     * Does a regular search as found in a normal List
     * @param index
     * @return The search result
     */
    searchResult regularSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);

        //Trailing JumpPointer must always be the previous jumpPointer
        //, therefore we use
        //the head when iterating forwards, but the penultimate one when iterating backwards.
        r.jumpPointer= forward ?
                jumpList.getHead():
                jumpList.getLastNode();

        if (index < jumpList.getSize() * distance - 1 && index > jumpList.getSize() * distance - distance - 1)
            r.jumpPointer= r.jumpPointer->getPrevNode();

        //Regular search algorithm
        r.node = forward ?
                this->searchFromFront(index):
                this->searchFromBack(index);

        setTrailingPointer(index, r.node, r.jumpPointer);
        return r;
    }

    /**
     * Is index 0? Then return the first node.
     * Is index n - 1? Then return the last node.
     * Is index to be searched regularly? Return a regular search result.
     * Is index out of range? Then return nullptr as a result; TrailingPointer is not modified.
     * @param index
     * @return The search result and confirmation if a result has been found
     */
    searchCheck constantSearchCheck(int index) {
        searchCheck check;
        if (index == 0) {
            check.r.node = this->getFirstNode();
            check.r.jumpPointer= jumpList.getHead();
            check.done = true;
            setTrailingPointer(index, check.r.node, check.r.jumpPointer);
        } else if (index == this->getMaxIndex()) {
            check.r.node = this->getLastNode();
            check.r.jumpPointer= jumpList.getLastNode();
            check.done = true;
            setTrailingPointer(index, check.r.node, check.r.jumpPointer);
        } else if (index < 0 || index > this->getMaxIndex()) {
            check.r.node = nullptr;
            check.r.jumpPointer= nullptr;
            check.done = true;
        } return check;
    }

    /**
     * May be referred to as 'QuickSearch', is the heart of the QuickList. Searches for the given index by using
     * the QuickList's JumpList and TrailingPointer to speed up the search.
     * @param index
     * @return The search result
     */
    searchResult search(int index) {
        //Check if index is in range and if it should return first or last node instead
        searchCheck check = constantSearchCheck(index);
        if (check.done)
            return check.r;

        //Check if using the TrailingPointer to navigate the QuickList is most efficient
        if (trailingPointerViable(index))
            //Do a Trailing search if it is cost-efficient
            return trailingSearch(index);
        else
            //Do a Non-Trailing search if starting from the head or tail is cost-efficient
            return nonTrailingSearch(index);
    }

    /**
     * Checks if appending or prepending is more efficient. Does so if it is.
     * @param index
     * @param data
     * @return False if the index has to be searched
     */
    bool addCheck(int index, T data) {
        if (index - 1 >= this->getMaxIndex()) {
            this->append(data);
            return true;
        }
        if (index <= 0) {
            this->prepend(data);
            if (!jumpList.isEmpty())
                jumpList.leftPointerShift(distance, index, jumpList.getFirstNode());
            return true;
        }
        return false;
    }

    /**
     * Adds a new node to the list taking the given index by means of QuickSearch and shifts JumpPointers to the left.
     * @param index
     * @param data
     */
    void add(int index, T data) {
        if (addCheck(index, data))
            return;

        searchResult r = search(index);
        this->linkUpNode(new Node<T>, r.node, data);
        jumpList.leftPointerShift(distance, index, r.jumpPointer);
        setTrailingPointer(index, r.node->getPrevNode(), r.jumpPointer);
    }

    void prepend(T data) override {
        auto* node = new Node<T>;
        node->setData(data);

        node->setPrevNode(this->getHead());
        node->setNextNode(this->getFirstNode());

        this->getHead()->setNextNode(node);
        node->getNextNode()->setPrevNode(node);

        jumpList.leftPointerShift(distance, 0, jumpList.getFirstNode());
        incSize();
    }

    /**
     * Checks if removing first or last node is more efficient. Does so if it is.
     * @param index
     * @return False if index has to be searched
     */
    bool removeCheck(int index) {
        if (index < 0 || index > this->getMaxIndex())
            return true;
        if (index == 0) {
            if (!jumpList.isEmpty())
                jumpList.rightPointerShift(distance, index, jumpList.getFirstNode());
            this->removeFirst();
            return true;
        } else if (index == this->getMaxIndex()) {
            this->removeLast();
            return true;
        }
        return false;
    }

    /**
     * Removes the node at the given index by means of QuickSearch and shifts JumpPointers to the right.
     * @param index
     */
    void remove(int index) override {
        invalidateTrailingPointer(index);
        if (removeCheck(index))
            return;

        searchResult r = search(index);
        jumpList.rightPointerShift(distance, index, r.jumpPointer);
        this->removeNode(r.node);
    }

    /**
     * Checks if removing a range can be made more efficient. Does so if it is.
     * @param indexStart
     * @param indexEnd
     * @return
     */
    bool rangeCheck(int indexStart, int indexEnd) {
        if (indexStart > this->getMaxIndex())
            return true;
        if (indexStart < 0)
            indexStart = 0;
        if (indexEnd < indexStart) {
            int temp = indexStart;
            indexStart = indexEnd;
            indexEnd = temp;
        }
        if (indexStart == indexEnd) {
            remove(indexStart);
            return true;
        }
        if (indexEnd > this->getMaxIndex()) {
            indexEnd = this->getMaxIndex();
            for (int i = 0; i <= indexEnd - indexStart; i++)
                this->removeLast();
            return true;
        }
        return false;
    }

    /**
     * Uses QuickSearch to remove given range from index to index (both inclusive).
     * Works like an iterator and is therefore much faster if removing multiple sequential nodes.
     * The JumpList will be forcibly rebuilt by this method, which can be costly.
     * @param indexStart The first node by index (inclusive)
     * @param indexEnd The last node by index (inclusive)
     */
    void removeRange(int indexStart, int indexEnd) {
        if (rangeCheck(indexStart, indexEnd))
            return;

        searchResult r = search(indexStart);
        Node<T>* node;
        for (int i = 0; i <= indexEnd - indexStart; i++) {
            node = r.node->getNextNode();
            r.node->unlink();
            delete r.node;
            r.node = node;
        }
        this->size -= indexEnd - indexStart;
        rebuildJumpList();
    }

    /**
     * Sets the data of the node at the given index to the given data value by using QuickSearch.
     * @param index
     * @param data
     */
    void set(int index, T data) override {
        searchResult r = search(index);
        r.node->setData(data);
    }

    /**
     * Gets the data of the node at the given index by using QuickSearch.
     * @param index
     * @return
     */
    T get(int index) override {
        searchResult r = search(index);
        return r.node->getData();
    }
};
