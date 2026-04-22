# Interval Tree-Based Meeting Scheduler

## Project Goal
The goal of this project is to implement an efficient meeting scheduling system that can handle booking, conflict detection, and cancellation of meetings across multiple rooms. The system must quickly identify overlapping time slots to prevent double-bookings while supporting operations like inserting new meetings, checking for conflicts, and removing existing bookings.

## Approach to Achieving the Goal
We are building this system using an Interval Tree data structure, which is an augmented binary search tree optimized for interval overlap queries. The tree stores meeting intervals (start and end times) and uses a "maxEnd" field in each node to enable efficient pruning of subtrees during overlap searches. This allows us to perform conflict checks in O(log n) time on average, much faster than linear scans.

The implementation follows a step-by-step development process, starting with core data structures and building up to the full scheduling application.

## Data Structures Used
- **Interval Struct**: Represents a time interval with start time, end time, and an optional label. Includes methods for overlap checking and string representation.
- **Node Struct**: Represents a node in the binary search tree, containing an Interval, a maxEnd value (maximum end time in the subtree), and left/right child pointers.
- **IntervalTree Class**: The main data structure that manages the tree, providing methods for insertion, overlap detection, and traversal.

## What We Have Done So Far
Following the step-by-step plan, we have completed the foundational components:

1. **Interval Struct**: Fully implemented with start/end times, label, overlap checking, and string conversion.
2. **Node Struct**: Implemented with interval storage, maxEnd tracking, and child pointers.
3. **updateMaxEnd Helper**: Function to recompute the maximum end time in a node's subtree after insertions.
4. **Insert Operation**: Recursive BST insertion by start time, with maxEnd updates.
5. **Overlap Search (findOverlap)**: Core algorithm that finds the first overlapping interval using maxEnd pruning.
6. **Tree Traversal**: In-order traversal for printing intervals in sorted order.
7. **IntervalTree Class**: Wrapper class with public methods for insertion, conflict checking, and printing.

The current implementation supports:
- Inserting intervals into the tree
- Checking if a new interval conflicts with existing ones
- Printing all intervals in sorted order

## What is Left to Do After the Interim Demo
To complete the full meeting scheduler system:

1. **findAllOverlaps**: Implement a method to find ALL overlapping intervals (not just the first), which visits both subtrees when necessary but still uses maxEnd for pruning.
2. **Delete Operation**: Implement BST deletion with three cases (no children, one child, two children), ensuring maxEnd is properly updated after removal.
3. **MeetingScheduler Class**: Create a wrapper class that manages multiple IntervalTrees (one per room), providing high-level methods for booking meetings (check conflicts before insert), canceling meetings (remove from tree), and querying room availability.
4. **Testing and Validation**: Expand the test suite to cover edge cases, multiple rooms, and all operations.
5. **Performance Optimization**: Add balancing (AVL/Red-Black) if needed for worst-case scenarios.
6. **User Interface**: Develop a simple command-line or GUI interface for interacting with the scheduler.

The next steps will focus on completing the delete operation and building the scheduler application layer.</content>
<parameter name="filePath">/home/mustafa/Habib/DS2/Project/DS2-Spring-2026-Project/README.md