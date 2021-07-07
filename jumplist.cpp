#include "baselist.cpp"

/**
 * The essence of QuickSearch. Stores JumpPointers that point to evenly spaced out nodes in the QuickList.
 */
template <typename T>
class JumpList : public BaseList<Node<T>*> {
public:
    /**
     * Has to be used in place of hasNext() to ensure that the tail of the JumpList will not be selected
     * @param jumpPointer
     * @return True if the jumpPointer is followed by another jumpPointer that is not the tail
     */
    bool hasNextJump(Node<Node<T>*>* jumpPointer) {
        return jumpPointer->getNextNode() != this->getTail();
    }

    Node<T>* getPrevNodeOfNextJumpData(Node<Node<T>*>* jumpPointer) {
        return jumpPointer->getNextNode()->getData()->getPrevNode();
    }

    Node<T>* getNextNodeOfNextJumpData(Node<Node<T>*>* jumpPointer) {
        return jumpPointer->getNextNode()->getData()->getNextNode();
    }

    /**
     * Shifts all affected JumpPointers to the left to accommodate for a new node in the QuickList
     * @param distance JumpPointer distance
     * @param index
     * @param jumpPointer The first affected JumpPointer
     */
    void leftPointerShift(int distance, int index, Node<Node<T>*>* jumpPointer) {
        if (jumpPointer == nullptr || jumpPointer == this->getHead() || jumpPointer == this->getTail())
            return;

        if (index < distance)
            jumpPointer->setData(jumpPointer->getData()->getPrevNode());

        while (this->hasNextJump(jumpPointer)) {
            jumpPointer->getNextNode()->setData(this->getPrevNodeOfNextJumpData(jumpPointer));
            jumpPointer = jumpPointer->getNextNode();
        }
    }

    /**
     * Shifts all affected JumpPointers to the right to accommodate for a removed node in the QuickList
     * @param distance JumpPointer distance
     * @param index
     * @param jumpPointer The first affected JumpPointer
     */
    void rightPointerShift(int distance, int index, Node<Node<T>*>* jumpPointer) {
        if (jumpPointer == this->getHead() || jumpPointer == this->getTail())
            return;

        if (index < distance)
            jumpPointer->setData(jumpPointer->getData()->getNextNode());

        while (this->hasNextJump(jumpPointer)) {
            jumpPointer->getNextNode()->setData(this->getNextNodeOfNextJumpData(jumpPointer));
            jumpPointer = jumpPointer->getNextNode();
        }
    }

    void debug_print(int distance)  {
        if (this->isEmpty()) {
            std::cout << "JumpList @" << this << " is empty\n";
            return;
        }

        Node<Node<T>*>* node = this->getFirstNode();
        int index = 0;
        while (this->hasNext(node)) {
            std::cout << "Index " << index << " -> Index " << index * distance + 9 << ":\t" << node->getData()->getData() << "\n";
            node = node->getNextNode();
            index++;
        }
        std::cout << "\n";
    }
};
