#include <iostream>
#include <cmath>
#include "baselist.cpp"


/**
 * The essence of QuickSearch. Stores JumpPointers that point to evenly spaced out nodes in the QuickList.
 */
template <typename T>
class JumpList : public BaseList<Node<T>*> {
public:
    /**
     * Has to be used in place of hasNext() to ensure that the tail of the JumpList will not be selected
     * @param jumpNode
     * @return True if the jumpNode is followed by another jumpNode that is not the tail
     */
    bool hasNextJump(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode() != this->getTail();
    }

    Node<T>* getPrevNodeOfNextJumpData(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode()->getData()->getPrevNode();
    }

    Node<T>* getNextNodeOfNextJumpData(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode()->getData()->getNextNode();
    }

    /**
     * Shifts all affected JumpPointers to the left to accommodate for a new node in the QuickList
     * @param distance JumpPointer distance
     * @param index
     * @param jumpNode The first affected JumpPointer
     */
    void leftPointerShift(int distance, int index, Node<Node<T>*>* jumpNode) {
        if (jumpNode == nullptr || jumpNode == this->getHead() || jumpNode == this->getTail())
            return;

        if (index < distance)
            jumpNode->setData(jumpNode->getData()->getPrevNode());

        while (this->hasNextJump(jumpNode)) {
            jumpNode->getNextNode()->setData(this->getPrevNodeOfNextJumpData(jumpNode));
            jumpNode = jumpNode->getNextNode();
        }
    }

    /**
     * Shifts all affected JumpPointers to the right to accommodate for a removed node in the QuickList
     * @param distance JumpPointer distance
     * @param index
     * @param jumpNode The first affected JumpPointer
     */
    void rightPointerShift(int distance, int index, Node<Node<T>*>* jumpNode) {
        if (jumpNode == this->getHead() || jumpNode == this->getTail())
            return;

        if (index < distance)
            jumpNode->setData(jumpNode->getData()->getNextNode());

        while (this->hasNextJump(jumpNode)) {
            jumpNode->getNextNode()->setData(this->getNextNodeOfNextJumpData(jumpNode));
            jumpNode = jumpNode->getNextNode();
        }
    }
};


template <typename T>
class QuickList : public BaseList<T> {
public:
    typedef struct TrailingPointer {
        int index;
        Node<Node<T>*>* jumpNode;
        Node<T>* node;
    } TrailingPointer;

    //JumpList provides the JumpPointers that point to individual nodes in the QuickList for fast access
    //This is the core functionality of a QuickList
    JumpList<T>* jumpList = new JumpList<T>;

    //TrailingPointer keeps track of the last used node and jumpNode to speed up (semi-)sequential rw operations
    TrailingPointer trailingPointer = {-1, nullptr, nullptr};

    //Distance between individual JumpPointers. Initial: 10
    //Always a multiple of 10, never below 10
    //Example: JumpPointers point to QuickList index 9, 19, 29, 39, 49,...
    int distance = 10;


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
        int x = (int) (-5 + sqrt(pow(5, 2) + ((double) this->getSize() / 2)));
        x -= x % 10;
        return x + 10;
    }

    /**
     * Calculates the upper critical size of the QuickList by using the distance
     * @return upper critical size value
     */
    int upperCritical() {
        return 2 * pow(distance, 2) + 20 * distance;
    }

    /**
     * Calculates the lower critical size of the QuickList by using the distance
     * @return lower critical size value
     */
    int lowerCritical() {
        return 2 * pow(distance, 2) - 20 * distance - 50;
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

    void setTrailingPointer(int index, Node<T>* node, Node<Node<T>*>* jumpNode) {
        trailingPointer.index = index;
        trailingPointer.node = node;
        trailingPointer.jumpNode = jumpNode;
    }

    bool trailingPointerViable(int index) {
        return
        trailingPointer.index != -1 &&
        abs(0 - index) > abs(trailingPointer.index - index) &&
        abs(this->getMaxIndex() - index) > abs(trailingPointer.index - index);
    }

    int trailingJumpPointerOffset(int index) {
        int x = trailingPointer.index - trailingPointer.index % distance;
        return -(x - index) / distance;
    }

    /**
     * Is returned by search(int index). Incorporates the searched node and the jumpNode it has been accessed from.
     */
    typedef struct searchResult {
        Node<T>* node;
        Node<Node<T>*>* jumpNode;
    } searchResult;

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
                for (int i = 1; i > jumpPointerOffset; i--)
                    r.jumpNode = r.jumpNode->getPrevNode();

            r.node = r.jumpNode->getData();

            for (int i = 0; i <= index % distance; i++)
                r.node = r.node->getNextNode();

            setTrailingPointer(index, r.node, r.jumpNode);
            return r;
        }
    }

    searchResult nonTrailingSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);

        //Decide starting point of search
        forward ?
        r.jumpNode = jumpList->getFirstNode():
        r.jumpNode = jumpList->getLastNode();

        //Check if jumpList should be iterated or if search should start at the head
        if (index >= distance) {
            //Depending on where search starts, the jumpList has to iterate forwards or backwards
            if (forward) {
                for (int i = 1; jumpList->hasNext(r.jumpNode); i++, r.jumpNode = r.jumpNode->getNextNode())
                    if (i == index / distance)
                        break;
            } else {
                for (int i = 0; jumpList->hasPrev(r.jumpNode); i++, r.jumpNode = r.jumpNode->getPrevNode())
                    if (i == jumpList->getSize() - (index / distance))
                        break;
            }

            //Sets node to the data of the determined jumpNode
            r.node = r.jumpNode->getData();
        } else
            r.node = this->getHead();

        //Iterate from pointed-to node to desired node
        for (int i = 0; i <= index % distance; i++)
            r.node = r.node->getNextNode();

        setTrailingPointer(index, r.node, r.jumpNode);
        return r;
    }

    searchResult regularSearch(int index) {
        searchResult r;
        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);
        //Regular search algorithm
        r.jumpNode = nullptr;
        r.node = forward ?
                this->searchFromFront(index):
                this->searchFromBack(index);

        setTrailingPointer(-1, r.node, r.jumpNode);
        return r;
    }

    typedef struct searchCheck {
        bool done = false;
        searchResult r {nullptr, nullptr};
    } sCheck;

    sCheck searchCheck(int index) {
        sCheck check;
        if (index == 0) {
            check.r.node = this->getFirstNode();
            check.r.jumpNode = nullptr;
            check.done = true;
            return check;
        } else if (index == this->getMaxIndex()) {
            check.r.node = this->getLastNode();
            check.r.jumpNode = nullptr;
            check.done = true;
            return check;
        } else if (index < 0 || index > this->getMaxIndex()) {
            check.r.node = nullptr;
            check.r.jumpNode = nullptr;
            check.done = true;
        } else return check;
    }

    /**
     * May be referred to as 'QuickSearch', is the heart of the QuickList. Searches for the given index by using
     * the QuickList's JumpList to speed up the search.
     * @param index
     * @return The search result
     */
    searchResult search(int index) {
        sCheck check = searchCheck(index);
        if (check.done)
            return check.r;

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
     * @param indexStart The first node by index
     * @param indexEnd The last node by index
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

/**
 * Just some tests. Should be removed for actual usage.
 * @return
 */
int main() {
    /*
    for (int i = 1; i <= 399; i++)
        quickList.append(i);

    for (int i = 1; i <= 800; i++)
        quickList.append(i);

    for (int i = 1; i <= 1200; i++)
        quickList.append(i);

    for (int i = 1; i <= 1601; i++)
        quickList.append(i);

    for (int i = 1; i <= 1649; i++)
        quickList.removeLast();

    for (int i = 1; i <= 1200; i++)
        quickList.removeLast();

    for (int i = 1; i <= 800; i++)
        quickList.removeLast();

    for (int i = 1; i <= 400; i++)
        quickList.removeLast();
        */
    QuickList<int> quickList;

    for (int i = 1; i <= 300; i++)
        quickList.append(i);

    quickList.search(180);
    quickList.search(185);
    quickList.search(203);
    quickList.search(154);
    quickList.search(154);
 }
