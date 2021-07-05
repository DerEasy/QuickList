#include <iostream>
#include "node.cpp"

template <typename T>
class BaseList {
public:
    int size = 0;

    Node<T>* head = new Node<T>;
    Node<T>* tail = new Node<T>;

    BaseList() {
        head->setNextNode(tail);
        tail->setPrevNode(head);
        head->setData({});
        tail->setData({});
    }

    ~BaseList() {
        Node<T>* node;
        while (hasNext(getFirstNode())) {
            node = getFirstNode();
            node->unlink();
            delete node;
        }
        delete head;
        delete tail;
    }

    int getSize() {
        return size;
    }

    int getMaxIndex() {
        return getSize() - 1;
    }

    bool isEmpty() {
        return getSize() == 0;
    }

    virtual bool useForwardSearch(int index) {
        return getSize() - index > getSize() / 2;
    }

    Node<T>* getHeadBoundary() {
        return head->prev;
    }

    Node<T>* getTailBoundary() {
        return tail->next;
    }

    Node<T>* getHead() {
        return head;
    }

    Node<T>* getTail() {
        return tail;
    }

    Node<T>* getFirstNode() {
        return head->next;
    }

    Node<T>* getLastNode() {
        return tail->prev;
    }

    bool hasNext(Node<T>* node) {
        return node->getNextNode() != getTailBoundary();
    }

    bool hasPrev(Node<T>* node) {
        return node->getPrevNode() != getHeadBoundary();
    }

    void setFirst(T data) {
        getFirstNode()->setData(data);
    }

    void setLast(T data) {
        getLastNode()->setData(data);
    }

    virtual void set(int index, T data) {
        getNode(index)->setData(data);
    }

    virtual T get(int index) {
        return getNode(index)->getData();
    }

    T getFirst() {
        return getFirstNode()->getData();
    }

    T getLast() {
        return getLastNode()->getData();
    }

    virtual void incSize() {
        size++;
    }

    virtual void decSize() {
        size--;
    }

    void clear() {
        while (hasNext(getFirstNode()))
            removeFirst();
    }

    void prepend(T data) {
        auto* node = new Node<T>;
        node->setData(data);

        node->setPrevNode(getHead());
        node->setNextNode(getFirstNode());

        getHead()->setNextNode(node);
        node->getNextNode()->setPrevNode(node);

        incSize();
    }

    void append(T data) {
        auto* node = new Node<T>;
        node->setData(data);

        node->setPrevNode(getLastNode());
        node->setNextNode(getTail());

        node->getPrevNode()->setNextNode(node);
        getTail()->setPrevNode(node);

        incSize();
    }

    void linkUpNode(Node<T>* node, Node<T>* nextNode, T data) {
        node->setData(data);
        node->setNextNode(nextNode);
        node->setPrevNode(nextNode->getPrevNode());
        node->setPrevOfNext(node);
        node->setNextOfPrev(node);
        incSize();
    }

    Node<T>* searchFromFront(int index) {
        Node<T>* node = getFirstNode();
        int i = 0;

        while (this->hasNext(node)) {
            if (i == index)
                return node;
            node = node->getNextNode();
            i++;
        }
        return getLastNode();
    }

    Node<T>* searchFromBack(int index) {
        Node<T>* node = getLastNode();
        int i = getMaxIndex();

        while (hasPrev(node)) {
            if (i == index)
                return node;
            node = node->getPrevNode();
            i--;
        }
        return getFirstNode();
    }

    void addFromFront(int index, T data) {
        linkUpNode(new Node<T>, searchFromFront(index), data);
    }

    void addFromBack(int index, T data) {
        linkUpNode(new Node<T>, searchFromBack(index), data);
    }

    void removeFirst() {
        if (isEmpty())
            return;
        removeNode(getFirstNode());
    }

    virtual void remove(int index) {
        if (isEmpty())
            return;
        removeNode(getNode(index));
    }

    void removeLast() {
        if (isEmpty())
            return;
        removeNode(getLastNode());
    }

    Node<T>* getNode(int index) {
        if (isEmpty())
            return getHead();

        if (index <= 0)
            return getFirstNode();
        else if (index >= getMaxIndex())
            return getLastNode();

        Node<T>* node;
        if (useForwardSearch(index)) {
            node = getFirstNode();
            for (int i = 0; hasNext(node) && i < index; i++)
                node = node->getNextNode();
        } else {
            node = getLastNode();
            for (int i = getMaxIndex(); hasPrev(node) && i > index; i--)
                node = node->getPrevNode();
        }
        return node;
    }

    void removeNode(Node<T>* node) {
        node->unlink();
        delete node;
        decSize();
    }

    int indexOf(T data) {
        Node<T>* node = getFirstNode();
        int index = 0;
        while (hasNext(node)) {
            if (node->getData() == data)
                return index;
            node = node->getNextNode();
            index++;
        }
        return -1;
    }

    int lastIndexOf(T data) {
        Node<T>* node = getLastNode();
        int index = getMaxIndex();
        while (hasPrev(node)) {
            if (node->getData() == data)
                return index;
            node = node->getPrevNode();
            index--;
        }
        return -1;
    }

    void removeFirstOccurrence(T data) {
        Node<T>* node = getFirstNode();
        while (hasNext(node)) {
            if (node->getData() == data) {
                removeNode(node);
                return;
            }
            node = node->getNextNode();
        }
    }

    void removeLastOccurrence(T data) {
        Node<T>* node = getLastNode();
        while (hasPrev(node)) {
            if (node->getData() == data) {
                removeNode(node);
                return;
            }
            node = node->getPrevNode();
        }
    }

    bool removeNthOccurrenceFromFront(T data, int n) {
        if (n < 1 || n > getSize())
            return false;

        int counter = 0;
        Node<T>* node = getFirstNode();
        while (hasNext(node)) {
            if (node->getData() == data)
                counter++;
            if (node->getData() == data && counter == n) {
                removeNode(node);
                return true;
            }
            node = node->getNextNode();
        }
        return false;
    }

    bool removeNthOccurrenceFromBack(T data, int n) {
        if (n < 1 || n > getSize())
            return false;

        int counter = 0;
        Node<T>* node = getLastNode();
        while (hasPrev(node)) {
            if (node->getData() == data)
                counter++;
            if (node->getData() == data && counter == n) {
                removeNode(node);
                return true;
            }
            node = node->getPrevNode();
        }
        return false;
    }

    bool removeAllOccurrences(T data) {
        bool hasRemoved = false;
        Node<T>* node = getFirstNode();
        while (hasNext(node)) {
            if (node->getData() == data) {
                node = node->getNextNode();
                removeNode(node->getPrevNode());
                hasRemoved = true;
                continue;
            }
            node = node->getNextNode();
        }
        return hasRemoved;
    }

    virtual void debug_print() {
        if (isEmpty()) {
            std::cout << "QuickList @" << this << " is empty\n";
            return;
        }

        Node<T>* node = getFirstNode();
        int index = 0;
        while (hasNext(node)) {
            std::cout << "Index " << index << ":\t" << node->getData() << "\n";
            node = node->getNextNode();
            index++;
        }
        std::cout << "\n";
    }

    virtual void debug_printReverse() {
        if (isEmpty()) {
            std::cout << "QuickList @" << this << " is empty\n";
            return;
        }

        Node<T>* node = getLastNode();
        int index = getMaxIndex();
        while (hasPrev(node)) {
            std::cout << "Index " << index << ":\t" << node->getData() << "\n";
            node = node->getPrevNode();
        }
        std::cout << "\n";
    }
};