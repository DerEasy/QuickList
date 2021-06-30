#include <iostream>
#include <cmath>
#include <chrono>
#include "jumplist.cpp"

template <typename T>
class QuickList : public BaseList<T> {
public:
    typedef struct TrailingPointer {
        int index;
        Node<Node<T>*>* jumpNode;
        Node<T>* node;
    } TrailingPointer;

    /**
    * Is returned by search(int index). Incorporates the searched node and the jumpNode it has been accessed from.
    */
    typedef struct searchResult {
        Node<T>* node;
        Node<Node<T>*>* jumpNode;
    } searchResult;

    typedef struct searchCheck {
        bool done = false;
        searchResult r {nullptr, nullptr};
    } sCheck;

    //JumpList provides the JumpPointers that point to individual nodes in the QuickList for fast access
    //This is the core functionality of a QuickList
    JumpList<T>* jumpList = new JumpList<T>;

    //TrailingPointer keeps track of the last used node and jumpNode to speed up (semi-)sequential rw operations
    TrailingPointer trailingPointer = {-1, nullptr, nullptr};

    //Distance between individual JumpPointers. Initial: 10
    //Always a multiple of 10, never below 10
    //Example: JumpPointers point to QuickList index 9, 19, 29, 39, 49,...
    int distance = 10;

    QuickList() {
        jumpList->getHead()->setData(this->getHead());
        jumpList->getTail()->setData(this->getTail());
    }

    ~QuickList() {
        delete jumpList;
    }

    /**
     * Increases the size of the QuickList and automatically rebuilds the JumpList or adds another JumpPointer if necessary
     */
    void incSize() override {
        this->size++;
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
    bool losesJumpPointer() {
        return this->getSize() % distance == distance - 1;
    }

    /**
     * Appends a JumpPointer to the JumpList
     */
    void addJumpPointer() {
        jumpList->append(this->getLastNode());
    }

    /**
     * Removes the last JumpPointer
     */
    void removeJumpPointer() {
        jumpList->removeLast();
    }

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
    int upperCritical() {
        return pow(distance, 2) + 10 * distance;
    }

    /**
     * Calculates the lower critical size of the QuickList by using the distance
     * @return lower critical size value
     */
    int lowerCritical() {
        return pow(distance, 2) - 10 * distance - 50;
    }

    /**
     * Attempts to rebuild the JumpList. Succeeds if upper or lower critical size has been reached.
     * @param forceRebuild True to force rebuilding the JumpList
     * @return True if JumpList has been rebuilt
     */
    bool rebuildJumpList() {
        if (this->getSize() >= upperCritical() || this->getSize() <= lowerCritical()) {
            jumpList->clear();
            distance = calcDistance();
            Node<T>* node = this->getFirstNode();

            int index = 1;
            while (this->hasNext(node)) {
                if (index % distance == 0)
                    jumpList->append(node);
                node = node->getNextNode();
                index++;
            }
            return true;
        }
        return false;
    }

    /**
     * Sets the values of the TrailingPointer
     * @param index Last used index
     * @param node Last used node
     * @param jumpNode Last used jumpNode
     */
    void setTrailingPointer(int index, Node<T>* node, Node<Node<T>*>* jumpNode) {
        trailingPointer.index = index;
        trailingPointer.node = node;
        trailingPointer.jumpNode = jumpNode;
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

    int getTrailingJumpPointerIndex() {
        if (!indexHasPointer(trailingPointer.index))
            return ((trailingPointer.index - trailingPointer.index % distance) / distance) - 1;
        return (((trailingPointer.index + 1) - (trailingPointer.index + 1) % distance) / distance) - 1;
    }

    /**
     * @param index The searched-for index
     * @return Offset for the TrailingPointer JumpPointer
     */
    int trailingJumpPointerOffset(int index) {
        return (index + 1) / distance - (trailingPointer.index + 1) / distance;

        /*
        if (indexHasPointer(index) && trailingPointer.index != index)
            return (((index - index % distance) / distance) - (getTrailingJumpPointerIndex() + 1)) + 1;
        else {
            if (index < trailingPointer.index)
                return ((index - index % distance) / distance) - (getTrailingJumpPointerIndex());
            return ((index - index % distance) / distance) - (getTrailingJumpPointerIndex() + 1);
        }
         */
    }

    int getJumpIndex(int index) {
        if (indexHasPointer(index))
            return index;
        else
            return (index - index % distance) - 1;
    }

    bool indexHasPointer(int index) {
        return (index + 1) % distance == 0;
    }

    bool backwardsTraversal(int index) {
        int jumpIndex = getJumpIndex(index);
        int nextJumpIndex = jumpIndex + distance;

        return index - jumpIndex > nextJumpIndex - index;
    }

    searchResult trailingSearch(int index) {
        searchResult r;
        r.jumpNode = trailingPointer.jumpNode;
        r.node = trailingPointer.node;

        int jumpPointerOffset = trailingJumpPointerOffset(index);

        if (jumpPointerOffset == 0) {
            int indexOffset = index - trailingPointer.index;

            if (indexOffset > 0)
                for (int i = 0; i < indexOffset; i++)
                    r.node = r.node->getNextNode();
            else if (index - trailingPointer.index < 0)
                for (int i = 0; i > indexOffset; i--)
                    r.node = r.node->getPrevNode();

            setTrailingPointer(index, r.node, r.jumpNode);
            return r;

        } else {
            if (jumpPointerOffset > 0)
                for (int i = 0; i < jumpPointerOffset; i++)
                    r.jumpNode = r.jumpNode->getNextNode();
            else
                for (int i = jumpPointerOffset; i < 0; i++)
                    r.jumpNode = r.jumpNode->getPrevNode();

            if (indexHasPointer(index)) {
                r.node = r.jumpNode->getData();
                setTrailingPointer(index, r.node, r.jumpNode);
                return r;
            }

            bool traverseJumpBack = backwardsTraversal(index);
            if (traverseJumpBack)
                r.jumpNode = r.jumpNode->getNextNode();

            r.node = r.jumpNode->getData();

            if (!traverseJumpBack) {
                for (int i = 0; i <= index % distance; i++)
                    r.node = r.node->getNextNode();
                setTrailingPointer(index, r.node, r.jumpNode);
            } else {
                for (int i = 0; i <= (distance - index % distance) - 2; i++)
                    r.node = r.node->getPrevNode();
                setTrailingPointer(index, r.node, r.jumpNode->getPrevNode());
            }

            return r;
        }
    }

    searchResult nonTrailingSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);
        bool traverseJumpBack = backwardsTraversal(index);

        //Decide starting point of search
        forward ?
        r.jumpNode = jumpList->getHead():
        r.jumpNode = jumpList->getTail();

        //Check if jumpList should be iterated or if search should start at the head
        //Depending on where search starts, the jumpList has to iterate forwards or backwards
        if (forward) {
            if (index < (distance - 1) - index)
                return regularSearch(index);

            for (int i = 0; jumpList->hasNext(r.jumpNode); i++, r.jumpNode = r.jumpNode->getNextNode())
                if (i == index / distance)
                    break;
        } else {
            if (index - getJumpIndex(index) > this->getSize() - index)
                return regularSearch(index);

            for (int i = -1; jumpList->hasPrev(r.jumpNode); i++, r.jumpNode = r.jumpNode->getPrevNode())
                if (i == jumpList->getSize() - (index / distance))
                    break;
        }

        if (index == getJumpIndex(index)) {
            r.jumpNode = r.jumpNode->getNextNode();
            r.node = r.jumpNode->getData();
            setTrailingPointer(index, r.node, r.jumpNode);
            return r;
        }
        if (traverseJumpBack)
            r.jumpNode = r.jumpNode->getNextNode();

        //Sets node to the data of the determined jumpNode
        r.node = r.jumpNode->getData();

        //Iterate from pointed-to node to desired node
        if (traverseJumpBack) {
            for (int i = 0; i <= (distance - index % distance) - 2; i++)
                r.node = r.node->getPrevNode();
            setTrailingPointer(index, r.node, r.jumpNode->getPrevNode());
        } else {
            for (int i = 0; i <= index % distance; i++)
                r.node = r.node->getNextNode();
            setTrailingPointer(index, r.node, r.jumpNode);
        }

        return r;
    }

    searchResult regularSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);
        //Regular search algorithm
        r.jumpNode = forward ?
                jumpList->getHead():
                jumpList->getTail();
        r.node = forward ?
                this->searchFromFront(index):
                this->searchFromBack(index);

        setTrailingPointer(index, r.node, r.jumpNode);
        return r;
    }

    sCheck searchCheck(int index) {
        sCheck check;
        if (index == 0) {
            check.r.node = this->getFirstNode();
            check.r.jumpNode = jumpList->getHead();
            check.done = true;
            setTrailingPointer(index, check.r.node, check.r.jumpNode);
            return check;
        } else if (index == this->getMaxIndex()) {
            check.r.node = this->getLastNode();
            check.r.jumpNode = jumpList->getTail();
            check.done = true;
            setTrailingPointer(index, check.r.node, check.r.jumpNode);
            return check;
        } else if (index < 0 || index > this->getMaxIndex()) {
            check.r.node = nullptr;
            check.r.jumpNode = nullptr;
            check.done = true;
        } return check;
    }

    /**
     * May be referred to as 'QuickSearch', is the heart of the QuickList. Searches for the given index by using
     * the QuickList's JumpList to speed up the search.
     * @param index
     * @return The search result
     */
    searchResult search(int index) {
        sCheck check = searchCheck(index);
        if (check.done) {

            return check.r;
        }

        //Only do a QuickSearch procedure if the jumpList is not empty
        if (!jumpList->isEmpty())
            if (trailingPointerViable(index))
                return trailingSearch(index);
            else
                return nonTrailingSearch(index);
        else
            return regularSearch(index);
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
            if (!jumpList->isEmpty())
                jumpList->leftPointerShift(distance, index, jumpList->getFirstNode());
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
        jumpList->leftPointerShift(distance, index, r.jumpNode);
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
            if (!jumpList->isEmpty())
                jumpList->rightPointerShift(distance, index, jumpList->getFirstNode());
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
        if (removeCheck(index))
            return;

        searchResult r = search(index);
        jumpList->rightPointerShift(distance, index, r.jumpNode);
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

void testAllSearchTypes() {
    QuickList<int> q;

    for (int i = 0; i <= 300; i++)
        q.append(i);

    //Non-Trailing search
    q.search(2);
    q.search(297);

    //First and Last
    q.search(0);
    q.search(300);

    //Middle
    q.search(150);

    //JumpPointer
    q.search(159);
    q.search(139);
    q.search(199);

    //Trailing search
    q.search(224);
    q.search(227);

    //Non-Trailing search
    q.search(37);
    q.search(285);
    q.search(19);
}

void testQuickSearchPerformance() {
    QuickList<int> q;

    for (int i = 0; i <= 999999; i++)
        q.append(i);

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 80000; i++)
        q.search(100000 + i * 10);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto ms_int = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << ms_int.count() << "µs\n";
}

/**
 * Just some tests. Should be removed for actual usage.
 * @return
 */
int main() {
    testAllSearchTypes();
    testQuickSearchPerformance();
 }
