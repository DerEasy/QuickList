#include "quicklist.cpp"
#include "fstream"
#include <chrono>
#include <thread>

void testAllSearchTypes() {
    QuickList<int> q;

    for (int i = 0; i <= 300; i++)
        q.append(i);

    //Non-Trailing search
    q.search(2);
    q.search(297);

    //First and Last
    q.search(0);
    q.search(300);

    //Middle
    q.search(150);

    //JumpPointer
    q.search(159);
    q.search(139);
    q.search(199);

    //Trailing search
    q.search(224);
    q.search(227);
    q.search(220);

    //Non-Trailing search
    q.search(37);
    q.search(285);
    q.search(19);
}

bool testQuickSearchPerformance() {
    std::cout << "\nTesting QuickSearch speed...\n";
    std::cout << "TrailingPointer is force-invalidated after every subroutine.\n";

    auto* q = new QuickList<int>;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++)
        q->append(i);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs appension (1.000.000 nodes)\n";

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 50000; i++)
        q->search(i);
    t2 = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs constant access (index 0 to 49.999)\n";

    q->forceInvalidateTrailingPointer();

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 25000; i++)
        q->getFirstNode();
    for (int i = 0; i < 25000; i++)
        q->getLastNode();
    t2 = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs constant access (25.000 times each, first/last node)\n";

    q->forceInvalidateTrailingPointer();

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 50000; i++)
        q->search(100000 + ((i % 1250) * 640));
    t2 = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs trailing access (index 100.000, jumps of 640, 50.000 times with resets)\n";

    q->forceInvalidateTrailingPointer();

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 50000; i++)
        q->search(random() % 1000000);
    t2 = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs random access (50.000 times)\n";

    t1 = std::chrono::high_resolution_clock::now();
    delete q;
    t2 = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "\t" << duration.count() << "µs deletion (1.000.000 nodes + destructor)\n";

    std::cout << "QuickSearch speed test successful.\n";
    return true;
}

bool testQuickSearchAccuracy() {
    bool success = true;
    QuickList<int> q;
    std::cout << "\nTesting QuickSearch accuracy...\n";

    for (int i = 0; i < 1000000; i++)
        q.append(i);

    for (int i = 0; i < 25000; i++) {
        q.search(500000 + i);
        if (q.trailingPointer.index != 500000 + i ||
        q.trailingPointer.node->getData() != 500000 + i) {
            std::cout << "Constant access (next) error at i = " << i << "\n";
            q.forceInvalidateTrailingPointer();
            success = false;
            continue;
        }
    }

    for (int i = 0; i < 25000; i++) {
        q.search(524999 - i);
        if (q.trailingPointer.index != 524999 - i ||
        q.trailingPointer.node->getData() != 524999 - i) {
            std::cout << "Constant access (prev) error at i = " << i << "\n";
            q.forceInvalidateTrailingPointer();
            success = false;
            continue;
        }
    }

    for (int i = 0; i < 50000; i++) {
        q.search(100000 + ((i % 1250) * 640));
        if (q.trailingPointer.index != 100000 + ((i % 1250) * 640) ||
        q.trailingPointer.node->getData() != 100000 + ((i % 1250) * 640)) {
            std::cout << "Trailing access error at i = " << i << "\n";
            q.forceInvalidateTrailingPointer();
            success = false;
            continue;
        }
    }

    int r;
    for (int i = 0; i < 50000; i++) {
        r = random() % 1000000;
        q.search(r);
        if (q.trailingPointer.index != r  ||
        q.trailingPointer.node->getData() != r) {
            std::cout << "Random access error at i = " << i << "\n";
            success = false;
            continue;
        }
    }

    if (success)
        std::cout << "QuickSearch accuracy test successful.\n";
    else
        std::cout << "QuickSearch accuracy test failed.\n";
    return success;
}

bool testAdd() {
    QuickList<int> q;
    std::cout << "\nTesting QuickList add function...\n";

    for (int i = 0; i < 300; i++)
        q.append(i);

    for (int i = 0; i <= 50; i++)
        q.add(49, 10050 - i);

    Node<int>* node = q.getFirstNode();
    int index = 0;
    int data = 0;
    int addData = 10000;
    bool success = true;
    std::ofstream log;
    log.open("log_add.txt");
    while (q.hasNext(node)) {
        if (data < 49 || addData > 10050) {
            if (node->getData() != data) {
                std::cout << "Error at index " << index << ": "
                << node->getData() << " should be " << data << "\n";
                success = false;
            }
            data++;
        } else if (addData <= 10050) {
            if (node->getData() != addData) {
                std::cout << "Error at index " << index << ": "
                << node->getData() << " should be " << addData << "\n";
                success = false;
            }
            addData++;
        }
        log << "Index " << index << ": " << node->getData() << "\n";
        node = node->getNextNode();
        ++index;
    }
    log.close();

    if (success)
        std::cout << "QuickList adding test successful.\n";
    else
        std::cout << "QuickList adding test failed. Check log file.\n";
    return success;
}

bool testPrepend() {
    QuickList<int> q;
    bool success = true;
    std::cout << "\nTesting QuickList prepend function...\n";

    for (int i = 0; i < 300; ++i)
        q.append(i);

    for (int i = 0; i < 500; ++i)
        q.prepend(1000 + i);

    int index = 0;
    Node<int>* node = q.getFirstNode();
    for (int i = 1499; i >= 1000; --i, ++index, node = node->getNextNode()) {
        if (node->getData() != i) {
            std::cout << "Prepension error at index " << index << "\n";
            success = false;
        }
    }
    for (int i = 0; i < 300; ++i, ++index, node = node->getNextNode()) {
        if (node->getData() != i) {
            std::cout << "Prepension error at index " << index << "\n";
            success = false;
        }
    }

    if (success)
        std::cout << "QuickList prepension test successful.\n";
    else
        std::cout << "QuickList prepension test failed.\n";
    return success;
}

void testRegularSearch() {
    QuickList<int> q;

    for (int i = 0; i <= 300; i++)
        q.append(i);

    q.search(56);
    q.search(15);
}

void testTrailingSearch() {
    QuickList<int> q;

    for (int i = 0; i <= 300; i++)
        q.append(i);

    q.search(157);
    q.search(145);
}

void runTests() {
    int testAmount = 4;
    int successfulTests = 1;

    //Performance test can't be failed
    testQuickSearchPerformance();
    if (testQuickSearchAccuracy()) ++successfulTests;
    if (testAdd()) ++successfulTests;
    if (testPrepend()) ++successfulTests;

    std::cout << "\n" << successfulTests << " of " << testAmount << " tests successful.\n";
}

int main() {
    runTests();
}