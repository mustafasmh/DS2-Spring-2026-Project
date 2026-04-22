#include <iostream>  // Include for input/output streams
#include "intervalTree.h"  // Include the header file for IntervalTree class
using namespace std;  // Use standard namespace

int main() {  // Main function, entry point of the program
    IntervalTree tree;  // Create an instance of IntervalTree

    tree.insert(Interval(10, 20, "A"));  // Insert interval [10,20] with label "A"
    tree.insert(Interval(5,  15, "B"));  // Insert interval [5,15] with label "B"
    tree.insert(Interval(25, 35, "C"));  // Insert interval [25,35] with label "C"

    cout << "=== All intervals ===\n";  // Print header for all intervals
    tree.print();  // Print all intervals in the tree

    cout << "\n=== hasConflict [12, 18] ===\n";  // Print header for conflict check
    cout << (tree.hasConflict(Interval(12, 18)) ? "Conflict found" : "No conflict") << "\n";  // Check if [12,18] conflicts with any interval and print result

    cout << "\n=== hasConflict [40, 50] ===\n";  // Print header for another conflict check
    cout << (tree.hasConflict(Interval(40, 50)) ? "Conflict found" : "No conflict") << "\n";  // Check if [40,50] conflicts and print result
    

    return 0;  // Return 0 to indicate successful execution
}