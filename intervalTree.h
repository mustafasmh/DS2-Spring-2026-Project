

#ifndef INTERVAL_TREE_H
#define INTERVAL_TREE_H

// Interval Tree Implementation
// This file defines the Interval struct used in an interval tree data structure.
// The Interval struct represents a range with start and end points, and an optional label.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Interval {
    int start;  // The starting point of the interval
    int end;    // The ending point of the interval
    string label; // An optional label for the interval

    // Constructor to initialize an interval with start, end, and optional label
    Interval(int s, int e, string lbl = ""){
        start = s;
        end   = e;
        label = lbl;
    }

    // Method to check if this interval overlaps with another interval
    bool overlapsWith(const Interval& other) const {
        return start < other.end && end > other.start;
    }

    // Method to convert the interval to a string representation
    string toString() const {
        return "[" + to_string(start) + ", " + to_string(end) + "] " + label;
    }
};

// Node represents one tree node in the interval tree.
// It stores the interval, the maximum end value in its subtree, and child pointers.
struct Node {
    Interval interval;
    int maxEnd;
    Node* left;
    Node* right;

    Node(Interval i)
        : interval(i), maxEnd(i.end), left(nullptr), right(nullptr) {}
};

// IntervalTree maintains the root node and supports interval insertion and overlap queries.
// It uses the 'maxEnd' field in each node to skip entire subtrees that cannot overlap.
class IntervalTree {
    private:
        Node* root; // Root of the interval tree

        int getMaxEnd(Node* n) {
            return n ? n->maxEnd : 0;
        }

        // Recompute the subtree max end value after inserting into children.
        void updateMaxEnd(Node* n) {
            if (!n) return;
            n->maxEnd = max({n->interval.end,
                             getMaxEnd(n->left),
                             getMaxEnd(n->right)});
        }

        // Recursively insert an interval into the tree by interval.start.
        Node* insert(Node* node, Interval interval){
            if (!node) return new Node(interval);

            if (interval.start < node->interval.start)
                node->left = insert(node->left, interval);
            else
                node->right = insert(node->right, interval);

            updateMaxEnd(node);
            return node;
        }

        // Find any interval in the subtree that overlaps the query.
        Node* findOverlap(Node* node, const Interval& query) {
            // If this subtree is empty, no overlap exists here.
            if (!node) return nullptr;

            // If the current node's interval overlaps the query, return it immediately.
            if (node->interval.overlapsWith(query))
                return node;

            // If the left subtree has any interval that ends after the query start,
            // then it may contain an overlapping interval. Search there first.
            if (node->left && getMaxEnd(node->left) > query.start)
                return findOverlap(node->left, query);

            // Otherwise, skip the left subtree and search the right subtree.
            return findOverlap(node->right, query);
        }

        void inOrder(Node* node){
            if (!node) return;
            inOrder(node->left);
            cout << node->interval.toString() << "\n";
            inOrder(node->right);
        }
    public:
    IntervalTree() : root(nullptr) {}

    // Insert a new interval into the interval tree.
    void insert(Interval interval) {
        root = insert(root, interval);
    }

    // Check whether the query interval overlaps with any existing interval.
    bool hasConflict(const Interval& query) {
        return findOverlap(root, query) != nullptr;
    }

    void print() {
        if (!root) {
            cout << "(empty)\n";
            return;
        }

        inOrder(root);
    }


    ~IntervalTree() {}
};
#endif