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
