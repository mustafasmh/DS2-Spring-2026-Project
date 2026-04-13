#include <iostream>
#include "intervalTree.h"
using namespace std;

int main() {
    IntervalTree tree;

    tree.insert(Interval(10, 20, "A"));
    tree.insert(Interval(5,  15, "B"));
    tree.insert(Interval(25, 35, "C"));

    cout << "=== All intervals ===\n";
    tree.print();

    cout << "\n=== hasConflict [12, 18] ===\n";
    cout << (tree.hasConflict(Interval(12, 18)) ? "Conflict found" : "No conflict") << "\n";

    cout << "\n=== hasConflict [40, 50] ===\n";
    cout << (tree.hasConflict(Interval(40, 50)) ? "Conflict found" : "No conflict") << "\n";

    

    return 0;
}