#include <iostream>
#include "intervalTree2.h"
using namespace std;

int main() {
    IntervalTree tree;

    // Insert intervals
    tree.insert(Interval(10, 20, "A"));
    tree.insert(Interval(5, 15, "B"));
    tree.insert(Interval(25, 35, "C"));
    tree.insert(Interval(12, 18, "D"));
    tree.insert(Interval(30, 40, "E"));

    cout << "=== All intervals initially ===\n";
    tree.print();

    // Conflict tests
    cout << "\n=== hasConflict [12, 18] ===\n";
    cout << (tree.hasConflict(Interval(12, 18)) ? "Conflict found" : "No conflict") << "\n";

    cout << "\n=== hasConflict [40, 50] ===\n";
    cout << (tree.hasConflict(Interval(40, 50)) ? "Conflict found" : "No conflict") << "\n";

    // Exact search tests
    cout << "\n=== Search [12, 18] ===\n";
    cout << (tree.search(12, 18) ? "Interval found" : "Interval not found") << "\n";

    cout << "\n=== Search [100, 200] ===\n";
    cout << (tree.search(100, 200) ? "Interval found" : "Interval not found") << "\n";

    // Delete test
    cout << "\n=== Delete [12, 18] ===\n";
    tree.remove(12, 18);
    tree.print();

    cout << "\n=== Search [12, 18] after deletion ===\n";
    cout << (tree.search(12, 18) ? "Interval found" : "Interval not found") << "\n";

    // Update test
    cout << "\n=== Update [25, 35] to [28, 50] ===\n";
    tree.update(25, 35, 28, 50, "C-updated");
    tree.print();

    cout << "\n=== Search [25, 35] after update ===\n";
    cout << (tree.search(25, 35) ? "Interval found" : "Interval not found") << "\n";

    cout << "\n=== Search [28, 50] after update ===\n";
    cout << (tree.search(28, 50) ? "Interval found" : "Interval not found") << "\n";

    return 0;
}