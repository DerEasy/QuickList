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

    void pointerShift(int distance, int index, Node<Node<T>*>* jumpNode) {
        if (index < distance)
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
    int distance = 10;
    int maxJump = 1;

    bool getsQuickPointer() {
        return BaseList<T>::getSize() != 0 && BaseList<T>::getSize() % distance == 0;
    }

    bool losesQuickPointer() {
        return BaseList<T>::getSize() % distance == distance - 1;
    }

    void addQuickPointer() {
        jumpList->append(this->getLastNode());
    }

    void removeQuickPointer() {

    }

    void adjustDistance() {
        if (this->getSize() >= 400 * maxJump + 50) {
            Node<Node<T>*>* jumpNode = jumpList->getHead();
            Node<Node<T>*>* deletableNode;

            if (distance == 10) {
                while (jumpList->hasNextJump(jumpNode)) {
                    jumpNode = jumpNode->getNextNode()->getNextNode();
                    deletableNode = jumpNode->getPrevNode();
                    deletableNode->unlink();
                    jumpList->decSize();
                    delete deletableNode;
                }

                maxJump += (distance + 10) / 10;
                distance += 10;
            } else {
                Node<T>* node = this->getFirstNode();

                maxJump += (distance + 10) / 10;
                distance += 10;

                jumpList->clear();
                int index = 1;
                while (this->hasNext(node)) {
                    if (index % distance == 0)
                        jumpList->append(node);

                    node = node->getNextNode();
                    index++;
                }
            }
        }
    }

    void incSize() override {
        BaseList<T>::size++;

        adjustDistance();

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
                jumpList->pointerShift(distance, index, jumpList->getFirstNode());
                return;
            }

        bool forward = this->useForwardSearch(index);

        if (!jumpList->isEmpty()) {
            Node<T>* nextNode;
            Node<Node<T>*>* jumpNode;

            forward ? jumpNode = jumpList->getFirstNode() : jumpNode = jumpList->getLastNode();

            if (index >= distance) {
                if (forward) {
                    for (int i = 1; jumpList->hasNext(jumpNode); i++, jumpNode = jumpNode->getNextNode())
                        if (i == index / distance)
                            break;
                } else {
                    for (int i = 0; jumpList->hasPrev(jumpNode); i++, jumpNode = jumpNode->getPrevNode())
                        if (i == jumpList->getSize() - (index / distance))
                            break;
                }

                nextNode = jumpNode->getData();
            } else
                nextNode = this->getHead();

            for (int i = 0; i <= index % distance; i++)
                nextNode = nextNode->getNextNode();

            this->addMiddle(new Node<T>, nextNode, data);
            jumpList->pointerShift(distance, index, jumpNode);
        } else {
            forward ?
            this->addFromFront(index, new Node<T>, this->getFirstNode(), data):
            this->addFromBack(index, new Node<T>, this->getLastNode(), data);
        }
    }
};

int main() {
    std::cout << "Filling list with 10.000.000 objects\n";
    List<int> list;
    for (int i = 1; i <= 10000000; i++)
        list.append(i);

    std::cout << "Initialized\n" <<
    "Adding 1000 nodes in middle of list...\n";

    for (int index = 4958700; index < 4959700; index++)
        list.add(index, 99999999);

    std::cout << "Done";

    //list.print();
}
