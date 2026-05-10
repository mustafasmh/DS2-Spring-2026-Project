

#ifndef INTERVAL_TREE_H  // Header guard to prevent multiple inclusions of this header file
#define INTERVAL_TREE_H  // Define the header guard macro

// Interval Tree Implementation
// This file defines the Interval struct used in an interval tree data structure.
// The Interval struct represents a range with start and end points, and an optional label.
// The IntervalTree class implements an interval tree for efficient overlap queries.

// Include necessary headers
#include <iostream>  // Include for input/output streams (cout, etc.)
#include <vector>    // Include for vector container (though not used here, possibly for future extensions)
#include <string>    // Include for string class
#include <algorithm> // Include for algorithms like max
using namespace std;  // Use the standard namespace to avoid prefixing std::

// Define the Interval struct to represent an interval with start, end, and label
struct Interval {
    int start;  // Integer representing the starting point of the interval
    int end;    // Integer representing the ending point of the interval
    string label; // String label for identifying or describing the interval

    // Constructor: Initializes the interval with start, end, and optional label (default empty)
    Interval(int s, int e, string lbl = ""){  // Parameters: s=start, e=end, lbl=label
        start = s;  // Assign start value
        end   = e;  // Assign end value
        label = lbl;  // Assign label value
    }

    // Method to check if this interval overlaps with another interval
    // Overlap condition: this.start < other.end && this.end > other.start
    bool overlapsWith(const Interval& other) const {  // const method, doesn't modify object
        return start < other.end && end > other.start;  // Return true if intervals overlap
    }

    // Method to convert the interval to a string representation for display
    string toString() const {  // const method
        return "[" + to_string(start) + ", " + to_string(end) + "] " + label;  // Format: [start, end] label
    }
};

// Define the Node struct for the binary search tree nodes in the interval tree
struct Node {
    Interval interval;  // The interval stored in this node
    int maxEnd;         // Maximum end value in the subtree rooted at this node (for optimization)
    Node* left;         // Pointer to left child node
    Node* right;        // Pointer to right child node

    // Constructor: Initialize node with an interval, set maxEnd to interval.end, children to null
    Node(Interval i)  // Parameter: i=interval to store
        : interval(i), maxEnd(i.end), left(nullptr), right(nullptr) {}  // Member initializer list: initialize interval, maxEnd, left, right
};

// IntervalTree maintains the root node and supports interval insertion and overlap queries.
// It uses the 'maxEnd' field in each node to skip entire subtrees that cannot overlap.
class IntervalTree {
    private:
        Node* root; // Root of the interval tree

        // Helper function to get the maxEnd of a node, returns 0 if node is null
        int getMaxEnd(Node* n) {
            return n ? n->maxEnd : 0;  // If n is not null, return n->maxEnd, else 0
        }

        // Find the node with minimum start value in a subtree (leftmost node).
        // Used to find the in-order successor when deleting a node with two children.
        Node* findMin(Node* root) {
            if (!root) return nullptr;  // If root is null, return nullptr
            // Keep going left until we find the leftmost node (minimum start value)
            while (root && root->left)
                root = root->left;  // Move to left child
            return root;  // Return the node with minimum start value
        }

        // Recompute the subtree max end value after inserting into children.
        void updateMaxEnd(Node* n) {
            if (!n) return;  // If node is null, do nothing
            n->maxEnd = max({n->interval.end,  // Take the maximum of the node's interval end
                             getMaxEnd(n->left),  // left subtree maxEnd
                             getMaxEnd(n->right)});  // right subtree maxEnd
        }

        // Recursively insert an interval into the tree by interval.start.
        Node* insert(Node* node, Interval interval){
            if (!node) return new Node(interval);  // If current node is null, create new node with interval

            if (interval.start < node->interval.start)  // If new interval start is less than current node's start
                node->left = insert(node->left, interval);  // Insert into left subtree
            else
                node->right = insert(node->right, interval);  // Insert into right subtree

            updateMaxEnd(node);  // Update the maxEnd for this node after insertion
            return node;  // Return the node
        }

        // Recursively search for an interval with exact start and end values.
        // Returns pointer to the node if found, nullptr otherwise.
        Node* search(Node* node, int start, int end) {
            if (!node) return nullptr;  // Base case: reached leaf or empty subtree

            // Check if this node matches the search criteria (exact start and end)
            if (node->interval.start == start && node->interval.end == end)
                return node;  // Found exact match, return this node

            // BST search: go left if search start is less than node's start
            if (start < node->interval.start)
                return search(node->left, start, end);  // Search left subtree
            else
                return search(node->right, start, end);  // Search right subtree
        }

        // Recursively delete an interval with exact start and end values from the tree.
        // Handles all three BST deletion cases: no children, one child, two children.
        // Returns the modified subtree root.
        Node* deleteNode(Node* node, int start, int end) {
            if (!node) return nullptr;  // Base case: node doesn't exist

            // Navigate to the node to delete using BST property
            if (start < node->interval.start) {
                node->left = deleteNode(node->left, start, end);  // Target is in left subtree
            }
            else if (start > node->interval.start) {
                node->right = deleteNode(node->right, start, end);  // Target is in right subtree
            }
            else {
                // Found node with matching start; now check if end also matches
                if (node->interval.end != end) {
                    return node;  // End doesn't match, so this isn't the right interval
                }

                // CASE 1: Node has no children (leaf node)
                if (!node->left && !node->right) {
                    delete node;  // Free memory
                    return nullptr;  // Return nullptr to remove this node from tree
                }
                // CASE 2: Node has only a right child
                else if (!node->left) {
                    Node* temp = node->right;  // Save right child
                    delete node;  // Free the node
                    return temp;  // Return right child to replace this node
                }
                // CASE 2: Node has only a left child
                else if (!node->right) {
                    Node* temp = node->left;  // Save left child
                    delete node;  // Free the node
                    return temp;  // Return left child to replace this node
                }
                // CASE 3: Node has two children (most complex case)
                else {
                    // Find in-order successor (minimum node in right subtree)
                    Node* succ = findMin(node->right);  // Get successor
                    // Copy successor's data to current node
                    node->interval.start = succ->interval.start;  // Copy start
                    node->interval.end = succ->interval.end;  // Copy end
                    node->interval.label = succ->interval.label;  // Copy label
                    // Delete the successor node from right subtree
                    node->right = deleteNode(node->right, succ->interval.start, succ->interval.end);
                }
            }

            // After deletion, recompute maxEnd for this node (critical for interval tree correctness)
            updateMaxEnd(node);
            return node;  // Return the (possibly modified) subtree root
        }

        // Find any interval in the subtree that overlaps the query.
        Node* findOverlap(Node* node, const Interval& query) {
            // If this subtree is empty, no overlap exists here.
            if (!node) return nullptr;  // Return null if no node

            // If the current node's interval overlaps the query, return it immediately.
            if (node->interval.overlapsWith(query))  // Check if current interval overlaps
                return node;  // Return this node

            // If the left subtree has any interval that ends after the query start,
            // then it may contain an overlapping interval. Search there first.
            if (node->left && getMaxEnd(node->left) > query.start)  // If left exists and its maxEnd > query.start
                return findOverlap(node->left, query);  // Search left subtree

            // Otherwise, skip the left subtree and search the right subtree.
            return findOverlap(node->right, query);  // Search right subtree
        }

        // Traverse the tree in sorted order and print each interval.
        // This uses an in-order traversal of the binary search tree keyed by interval.start.
        void inOrder(Node* node){
            if (!node) return;  // If node is null, return
            inOrder(node->left);  // Traverse left subtree
            cout << node->interval.toString() << "\n";  // Print the interval
            inOrder(node->right);  // Traverse right subtree
        }

        // Find ALL intervals in the subtree that overlap the query interval.
        // Unlike findOverlap which returns on first match, this collects all overlaps.
        // It visits both subtrees when necessary but still prunes using maxEnd for efficiency.
        void findAllOverlaps(Node* node, const Interval& query, vector<Interval>& result) {
            if (!node) return;  // Base case: null node, nothing to search
            
            // Pruning optimization: if this subtree's maxEnd doesn't reach query.start,
            // no interval in this subtree can possibly overlap with query, so skip entirely
            if (node->maxEnd <= query.start) return;
            
            // If current node's interval starts at or after query ends,
            // no overlap possible at this node or in right subtree, only check left
            if (node->interval.start >= query.end) {
                findAllOverlaps(node->left, query, result);  // Search left subtree only
                return;  // Don't search right since all intervals there start even later
            }

            // Search left subtree for overlaps (may contain intervals ending in query range)
            findAllOverlaps(node->left, query, result);
            
            // Check if current node's interval overlaps the query
            if (node->interval.overlapsWith(query))
                result.push_back(node->interval);  // Add this interval to results
            
            // Search right subtree for overlaps (may contain intervals starting in query range)
            findAllOverlaps(node->right, query, result);
        }
    public:
    IntervalTree() : root(nullptr) {}  // Constructor: Initialize root to nullptr

    // Insert a new interval into the interval tree.
    void insert(Interval interval) {
        root = insert(root, interval);  // Call the private insert method starting from root
    }

    // Check whether the query interval overlaps with any existing interval.
    bool hasConflict(const Interval& query) {
        return findOverlap(root, query) != nullptr;  // Return true if findOverlap finds a node
    }

    // Public wrapper: Search for an interval with exact start and end values.
    // Returns true if interval exists, false otherwise.
    bool search(int start, int end) {
        return search(root, start, end) != nullptr;  // Call private helper and check if found
    }

    // Public wrapper: Remove an interval from the tree by its start and end values.
    // Updates root and recomputes maxEnd values after deletion.
    void remove(int start, int end) {
        root = deleteNode(root, start, end);  // Call private delete method and update root
    }

    // Public wrapper: Update an existing interval by deleting the old one and inserting a new one.
    // Parameters: oldStart (old interval start), oldEnd (old interval end),
    // newStart (new interval start), newEnd (new interval end), newLabel (new label, default empty).
    void update(int oldStart, int oldEnd, int newStart, int newEnd, string newLabel = "") {
        root = deleteNode(root, oldStart, oldEnd);  // Remove the old interval
        root = insert(root, Interval(newStart, newEnd, newLabel));  // Insert the new interval
    }

    // Print all intervals in the tree in sorted order (in-order traversal).
    void print() {
        if (!root) {
            cout << "(empty)\n";  // If tree is empty, print empty message
            return;  // Exit early
        }
        inOrder(root);  // Call inOrder to print all intervals in sorted order
    }

    // Public wrapper: Find and return all intervals that overlap with the query interval.
    // This method aggregates all conflicting intervals and returns them as a vector.
    // Time complexity: O(k + log n) where k = number of overlaps found, n = total intervals
    vector<Interval> findAllOverlaps(const Interval& query) {
        vector<Interval> result;  // Create empty result vector to collect all overlapping intervals
        findAllOverlaps(root, query, result);  // Call private recursive helper starting from root
        return result;  // Return the vector containing all overlapping intervals found
    }


    // Destructor: Cleans up all allocated nodes (currently empty, would need recursive cleanup for production).
    ~IntervalTree() {}
};
#endif