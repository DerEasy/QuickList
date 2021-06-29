template <typename T>
class Node {
public:
    T data;

    Node* prev;
    Node* next;

    T getData() {
        return data;
    }

    T getJumpData() {
        return data->data;
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