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

struct Node {
    Interval interval;
    int maxEnd;
    Node* left;
    Node* right;

    Node(Interval i)
        : interval(i), maxEnd(i.end), left(nullptr), right(nullptr) {}
};

class IntervalTree {
    private:
        Node* root;

        int getMaxEnd(Node* n) {
            return n ? n->maxEnd : 0;
        }

        void updateMaxEnd(Node* n) {
            if (!n) return;
            n->maxEnd = max({n->interval.end,
                             getMaxEnd(n->left),
                             getMaxEnd(n->right)});
        }

        Node* insert(Node* node, Interval interval){
            if (!node) return new Node(interval);

            if (interval.start < node->interval.start)
                node->left = insert(node->left, interval);
            else
                node->right = insert(node->right, interval);

            updateMaxEnd(node);
            return node;
        }
    public:
    IntervalTree() : root(nullptr) {}

    ~IntervalTree() {}
};