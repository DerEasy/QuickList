#include <iostream>
#include "jlist.cpp"

template <typename T>
class JumpList : public BaseList<Node<T>*> {
public:
    bool hasNextJump(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode() != this->getTail();
    }

    Node<T>* getPrevDataOfNext(Node<Node<T>*>* jumpNode) {
        return jumpNode->getNextNode()->getData()->getPrevNode();
    }

    void pointerShift(int index, Node<Node<T>*>* jumpNode) {
        if (index < 50)
            jumpNode->setData(jumpNode->getData()->getPrevNode());

        while (this->hasNextJump(jumpNode)) {
            jumpNode->getNextNode()->data = this->getPrevDataOfNext(jumpNode);
            jumpNode = jumpNode->getNextNode();
        }
    }
};

template <typename T>
class List : public BaseList<T> {
public:
    JumpList<T>* jumpList = new JumpList<T>;

    bool getsQuickPointer() {
        return BaseList<T>::getSize() != 0 && BaseList<T>::getSize() % 50 == 0 ;
    }

    bool losesQuickPointer() {
        return BaseList<T>::getSize() % 50 == 49;
    }

    void addQuickPointer() {
        jumpList->append(this->getLastNode());
    }

    void removeQuickPointer() {

    }

    void incSize() override {
        BaseList<T>::size++;

        if (getsQuickPointer())
            addQuickPointer();
    }

    void decSize() override {
        BaseList<T>::size--;
        if (losesQuickPointer())
            removeQuickPointer();
    }

    void add(int index, T data) {
        if (this->isLast(index, data) || this->isFirst(index, data))
            if (!jumpList->isEmpty()) {
                jumpList->pointerShift(index, jumpList->getFirstNode());
                return;
            }

        if (!jumpList->isEmpty()) {
            Node<T>* nextNode;
            Node<Node<T>*>* jumpNode;

            if (index >= 50) {
                if (this->useForwardSearch(index)) {
                    jumpNode = jumpList->getFirstNode();
                    for (int i = 1; jumpList->hasNext(jumpNode); i++, jumpNode = jumpNode->getNextNode())
                        if (i == index / 50)
                            break;
                } else {
                    jumpNode = jumpList->getLastNode();
                    for (int i = 0; jumpList->hasPrev(jumpNode); i++, jumpNode = jumpNode->getPrevNode())
                        if (i == jumpList->getSize() - (index / 50))
                            break;
                }

                nextNode = jumpNode->getData();
            } else
                nextNode = this->getHead();

            for (int i = 0; i <= index % 50; i++)
                nextNode = nextNode->getNextNode();

            this->addMiddle(new Node<T>, nextNode, data);
            jumpList->pointerShift(index, jumpNode);
        } else {
            auto* node = new Node<T>;

            if (this->useForwardSearch(index))
                this->addFromFront(index, node, this->getFirstNode(), data);
            else
                this->addFromBack(index, node, this->getLastNode(), data);
        }
    }
};

int main() {
    List<int> list;
    for (int i = 1; i <= 30000; i++)
        list.append(i);

    list.add(2, 2000);
    list.add(33, 33000);
    list.add(43, 43000);
    list.add(95, 95000);
    list.add(97, 97000);
    list.add(100, 100000);
    list.add(120, 120000);
    list.add(255, 255000);
    list.add(29000, 29000000);

    list.print();
}
