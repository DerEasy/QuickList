#include <iostream>

template <typename T>
class Node {
public:
    T data;

    Node* prev;
    Node* next;

    T getData() {
        return data;
    }

    void setData(T d) {
        data = d;
    }

    Node<T>* getNextNode() {
        return next;
    }

    Node<T>* getPrevNode() {
        return prev;
    }

    void setNextNode(Node<T>* node) {
        next = node;
    }

    void setPrevNode(Node<T>* node) {
        prev = node;
    }

    void setPrevOfNext(Node<T>* node) {
        getNextNode()->setPrevNode(node);
    }

    void setNextOfPrev(Node<T>* node) {
        getPrevNode()->setNextNode(node);
    }

    void unlink() {
        setNextOfPrev(next);
        setPrevOfNext(prev);
        next = nullptr;
        prev = nullptr;
    }
};

template <typename T>
class BaseList {
public:
    unsigned int size = 0;

    Node<T>* head = new Node<T>;
    Node<T>* tail = new Node<T>;

    int getSize() {
        return size;
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

    Node<T>* getSecond() {
        return getFirstNode()->getNextNode();
    }

    Node<T>* getPenultimate() {
        return getLastNode()->getPrevNode();
    }

    bool isHead(Node<T>* node) {
        return node == getHead();
    }

    bool isTail(Node<T>* node) {
        return node == getTail();
    }

    bool hasNext(Node<T>* node) {
        return node->getNextNode() != getTailBoundary();
    }

    bool hasPrev(Node<T>* node) {
        return node->getPrevNode() != getHeadBoundary();
    }

    virtual void incSize() {
        size++;
    }

    virtual void decSize() {
        size--;
    }

    BaseList() {
        head->setNextNode(tail);
        tail->setPrevNode(head);
        head->setData({});
        tail->setData({});
    }

    ~BaseList() {
        delete head;
        delete tail;
    }

    void clear() {
        while (hasNext(getFirstNode())) {
            removeFirst();
        }
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

    void removeFirst() {
        if (isEmpty())
            return;

        Node<T>* node = getFirstNode();
        getSecond()->setPrevNode(getHead());
        getHead()->setNextNode(getSecond());
        decSize();
        delete node;
    }

    void remove(int index) {
        if (isEmpty())
            return;

        Node<T>* node = getNode(index);
        node->unlink();
        decSize();
        delete node;
    }

    void removeLast() {
        if (isEmpty())
            return;

        Node<T>* node = getLastNode();
        getPenultimate()->setNextNode(getTail());
        getTail()->setPrevNode(getPenultimate());
        decSize();
        delete node;
    }

    Node<T>* getNode(int index) {
        if (isEmpty())
            return getHead();

        if (useForwardSearch(index)) {
            Node<T>* node = getFirstNode();
            for (int i = 0; hasNext(node) && i < index; i++)
                node = node->getNextNode();

            return node;
        } else {
            Node<T>* node = getLastNode();
            for (int i = getSize() - 1; hasPrev(node) && i > index; i--)
                node = node->getPrevNode();

            return node;
        }
    }

    void setData(int index, T data) {
        getNode(index)->data = data;
    }

    T getData(int index) {
        return getNode(index)->data;
    }

    void print() {
        if (isEmpty()) {
            std::cout << "List @" << this << " empty\n";
            return;
        }

        Node<T>* node = getFirstNode();
        int index = 0;
        while (hasNext(node)) {
            std::cout << index << ": " << node->getData() << "\n";
            node = node->getNextNode();
            index++;
        }
        std::cout << "\n";
    }

    void printReverse() {
        if (isEmpty()) {
            std::cout << "List @" << this << " empty\n";
            return;
        }

        Node<T>* node = getLastNode();
        while (hasPrev(node)) {
            std::cout << node->getData() << "\n";
            node = node->getPrevNode();
        }
        std::cout << "\n";
    }
};