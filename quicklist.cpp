#include <iostream>
#include <list>
#include <cmath>
#include "baselist.cpp"
#include "chrono"

using namespace std::chrono;

template <typename T>
class JumpList : public BaseList<Node<T>*> {
public:
    bool hasNextJump(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode() != this->getTail();
    }

    Node<T>* getPrevDataOfNext(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode()->getData()->getPrevNode();
    }

    Node<T>* getNextDataOfNext(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode()->getData()->getNextNode();
    }

    void leftPointerShift(int distance, int index, Node<Node<T>*>* jumpNode) {
        if (jumpNode == this->getHead() || jumpNode == this->getTail())
            return;

        if (index < distance)
            jumpNode->setData(jumpNode->getData()->getPrevNode());

        while (this->hasNextJump(jumpNode)) {
            jumpNode->getNextNode()->setData(this->getPrevDataOfNext(jumpNode));
            jumpNode = jumpNode->getNextNode();
        }
    }

    void rightPointerShift(int distance, int index, Node<Node<T>*>* jumpNode) {
        if (jumpNode == this->getHead() || jumpNode == this->getTail())
            return;

        if (index < distance)
            jumpNode->setData(jumpNode->getData()->getNextNode());

        while (this->hasNextJump(jumpNode)) {
            jumpNode->getNextNode()->setData(this->getNextDataOfNext(jumpNode));
            jumpNode = jumpNode->getNextNode();
        }
    }
};

template <typename T>
class QuickList : public BaseList<T> {
public:
    JumpList<T>* jumpList = new JumpList<T>;
    int distance = 10;

    void incSize() override {
        BaseList<T>::size++;
        rebuildJumpList();

        if (getsJumpPointer())
            addJumpPointer();
    }

    void decSize() override {
        BaseList<T>::size--;
        rebuildJumpList();

        if (losesJumpPointer())
            removeJumpPointer();
    }

    bool getsJumpPointer() {
        return BaseList<T>::getSize() != 0 && BaseList<T>::getSize() % distance == 0;
    }

    bool losesJumpPointer() {
        return BaseList<T>::getSize() % distance == distance - 1;
    }

    void addJumpPointer() {
        jumpList->append(this->getLastNode());
    }

    void removeJumpPointer() {
        jumpList->removeLast();
    }

    bool appendIfLast(int index, T data) {
        if (index - 1 >= this->getSize() - 1) {
            this->append(data);
            return true;
        }
        return false;
    }

    bool prependIfFirst(int index, T data) {
        if (index <= 0) {
            this->prepend(data);
            if (!jumpList->isEmpty())
                jumpList->leftPointerShift(distance, index, jumpList->getFirstNode());
            return true;
        }
        return false;
    }

    int calcDistance() {
        int x = (int) (-5 + sqrt(pow(5, 2) + ((double) this->getSize() / 2)));
        x -= x % 10;
        return x + 10;
    }

    int upperCritical() {
        return 2 * pow(distance, 2) + 20 * distance;
    }

    int lowerCritical() {
        return 2 * pow(distance, 2) - 20 * distance - 50;
    }

    void buildJumpList() {
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
    }

    void rebuildJumpList() {
        if (this->getSize() >= upperCritical()) {
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
        } else if (this->getSize() <= lowerCritical()) {
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
        }
    }

    typedef struct searchResult {
        Node<T>* node;
        Node<Node<T>*>* jumpNode;
    } searchResult;

    searchResult search(int index) {
        searchResult r;
        if (index <= 0) {
            r.node = this->getFirstNode();
            r.jumpNode = jumpList->getHead();
            return r;
        } else if (index >= this->getSize() - 1) {
            r.node = this->getLastNode();
            r.jumpNode = jumpList->getTail();
            return r;
        }

        bool forward = this->useForwardSearch(index);

        if (!jumpList->isEmpty()) {
            forward ?
            r.jumpNode = jumpList->getFirstNode():
            r.jumpNode = jumpList->getLastNode();

            if (index >= distance) {
                if (forward) {
                    for (int i = 1; jumpList->hasNext(r.jumpNode); i++, r.jumpNode = r.jumpNode->getNextNode())
                        if (i == index / distance)
                            break;
                } else {
                    for (int i = 0; jumpList->hasPrev(r.jumpNode); i++, r.jumpNode = r.jumpNode->getPrevNode())
                        if (i == jumpList->getSize() - (index / distance))
                            break;
                }

                r.node = r.jumpNode->getData();
            } else
                r.node = this->getHead();

            for (int i = 0; i <= index % distance; i++)
                r.node = r.node->getNextNode();

            return r;
        } else {
            r.jumpNode = jumpList->getHead();
            r.node = forward ?
            this->searchFromFront(index):
            this->searchFromBack(index);

            return r;
        }
    }

    void add(int index, T data) {
        if (this->appendIfLast(index, data) || this->prependIfFirst(index, data))
            return;

        searchResult r = search(index);

        this->addNode(new Node<T>, r.node, data);
        jumpList->leftPointerShift(distance, index, r.jumpNode);
    }

    void remove(int index) override {
        if (index <= 0) {
            this->removeFirst();
            if (!jumpList->isEmpty())
                jumpList->rightPointerShift(distance, index, jumpList->getFirstNode());
            return;
        } else if (index >= this->getSize() - 1) {
            this->removeLast();
            return;
        }

        searchResult r = search(index);

        jumpList->rightPointerShift(distance, index, r.jumpNode);
        r.node->unlink();
        delete r.node;
        decSize();
    }

    void removeRange(int indexStart, int indexEnd) {
        searchResult r = search(indexStart);
        Node<T>* node;
        for (int i = 0; i <= indexEnd - indexStart; i++) {
            node = r.node->getNextNode();
            r.node->unlink();
            delete r.node;
            r.node = node;
        }
        BaseList<T>::size -= indexEnd - indexStart;
        buildJumpList();
    }

    void set(int index, T data) override {
        searchResult r = search(index);
        r.node->setData(data);
    }

    T get(int index) override {
        searchResult r = search(index);
        return r.node->getData();
    }
};

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

    for (int i = 1; i <= 1000000; i++)
        quickList.append(i);
    auto t1 = high_resolution_clock::now();
    for (int i = 1; i <= 1000; i++)
        quickList.add(500000, 999999999);
    auto t2 = high_resolution_clock::now();

    auto result = duration_cast<milliseconds>(t2 - t1);
    std::cout << result.count() << "ms insertion\n";
}
