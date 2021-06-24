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
        if (jumpNode == this->getHead() || jumpNode == this->getTail())
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
    //JumpList provides the JumpPointers that point to individual nodes in the QuickList for fast access
    //This is the core functionality of a QuickList
    JumpList<T>* jumpList = new JumpList<T>;

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
        bool rebuilt = rebuildJumpList(false);

        if (!rebuilt && getsJumpPointer())
            addJumpPointer();
    }

    /**
     * Decreases the size of the QuickList and automatically rebuilds the JumpList or removes the last JumpPointer if necessary
     */
    void decSize() override {
        this->size--;
        rebuildJumpList(false);

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
    bool rebuildJumpList(bool forceRebuild) {
        if (forceRebuild || this->getSize() >= upperCritical() || this->getSize() <= lowerCritical()) {
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
     * Is returned by search(int index). Incorporates the searched node and the jumpNode it has been accessed from.
     */
    typedef struct searchResult {
        Node<T>* node;
        Node<Node<T>*>* jumpNode;
    } searchResult;

    /**
     * May be referred to as 'QuickSearch', is the heart of the QuickList. Searches for the given index by using
     * the QuickList's JumpList to speed up the search.
     * @param index
     * @return The search result
     */
    searchResult search(int index) {
        searchResult r;
        if (index <= 0) {
            r.node = this->getFirstNode();
            r.jumpNode = jumpList->getHead();
            return r;
        } else if (index >= this->getMaxIndex()) {
            r.node = this->getLastNode();
            r.jumpNode = jumpList->getTail();
            return r;
        }

        //Ask for jumpList iteration direction
        bool forward = this->useForwardSearch(index);

        //Only do a QuickSearch procedure if the jumpList is not empty
        if (!jumpList->isEmpty()) {
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

            return r;
        } else {
            //Regular search algorithm
            r.jumpNode = jumpList->getHead();
            r.node = forward ?
            this->searchFromFront(index):
            this->searchFromBack(index);

            return r;
        }
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
        rebuildJumpList(true);
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
    QuickList<int> quickList;
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

    for (int i = 1; i <= 42; i++)
        quickList.append(i);

    quickList.print();
    quickList.add(7, 555);
    quickList.add(16, 555);
    quickList.add(38, 555);
    quickList.print();
}
