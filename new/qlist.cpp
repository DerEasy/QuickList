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
        if (index >= this->getSize() - 1) {
            this->append(data);
            return;
        } else if (index <= 0) {
            this->prepend(data);
            return;
        }

        if (!jumpList->isEmpty()) {
            Node<T>* nextNode;
            Node<Node<T>*>* jumpNode = jumpList->getFirstNode();

            if (index >= 50) {
                int neededJump = index / 50;

                for (int i = 1; jumpList->hasNext(jumpNode); i++, jumpNode = jumpNode->getNextNode())
                    if (i == neededJump)
                        break;

                nextNode = jumpNode->getData();
            } else
                nextNode = this->getHead();

            for (int i = 0; i <= index % 50; i++)
                nextNode = nextNode->getNextNode();

            auto* node = new Node<T>;
            node->setData(data);
            node->setNextNode(nextNode);
            node->setPrevNode(nextNode->getPrevNode());
            node->setNextOfPrev(node);
            node->setPrevOfNext(node);
            incSize();

            if (index < 50)
                jumpNode->setData(jumpNode->getData()->getPrevNode());

            while (jumpList->hasNextJump(jumpNode)) {
                jumpNode->getNextNode()->data = jumpList->getPrevDataOfNext(jumpNode);
                jumpNode = jumpNode->getNextNode();
            }
        }
    }
};

int main() {
    List<int> list;
    for (int i = 1; i <= 300; i++)
        list.append(i);

    list.add(2, 2000);
    list.add(33, 33000);
    list.add(43, 43000);
    list.add(95, 95000);
    list.add(97, 97000);
    list.add(100, 100000);
    list.add(120, 120000);
    list.add(255, 255000);

    list.print();
}
