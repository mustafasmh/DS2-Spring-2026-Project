#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include "intervalTree.h"

using namespace std;

// Utility functions for converting between HH:MM strings and integer minutes
int timeToMins(const string& t) {
    if (t.length() != 5 || t[2] != ':') return -1;
    try {
        int h = stoi(t.substr(0, 2));
        int m = stoi(t.substr(3, 2));
        if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
        return h * 60 + m;
    } catch (...) {
        return -1;
    }
}

string minsToTime(int m) {
    int h = m / 60;
    int mins = m % 60;
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, mins);
    return string(buf);
}

// Encode and decode Title and Description into the single Interval label field
string encodeLabel(const string& title, const string& desc) {
    return title + "||" + desc;
}

void decodeLabel(const string& label, string& title, string& desc) {
    size_t pos = label.find("||");
    if (pos != string::npos) {
        title = label.substr(0, pos);
        desc = label.substr(pos + 2);
    } else {
        title = label;
        desc = "";
    }
}

class MeetingScheduler {
private:
    map<string, IntervalTree> rooms;
    vector<pair<string, Interval>> currentBookingsView;

    // Helper to print interval tree nodes recursively
    void printNodeTree(Node* node, int depth) {
        if (!node) return;
        
        // Traverse right first for a sideways tree view
        printNodeTree(node->right, depth + 1);

        string indent(depth * 6, ' ');
        string t, d;
        decodeLabel(node->interval.label, t, d);

        cout << indent << ">> [" << minsToTime(node->interval.start) << " to "
             << minsToTime(node->interval.end) << "] " << t
             << " (MaxEnd: " << minsToTime(node->maxEnd) << ")\n";

        printNodeTree(node->left, depth + 1);
    }

public:
    void addRoom() {
        cout << "Enter new room name: ";
        string name;
        getline(cin, name);
        if (rooms.find(name) != rooms.end()) {
            cout << "[ERROR] Room already exists.\n";
        } else {
            rooms[name] = IntervalTree();
            cout << "[SUCCESS] Room '" << name << "' added successfully.\n";
        }
    }

    void viewRooms() {
        cout << "\n=== All Rooms ===\n";
        if (rooms.empty()) {
            cout << "No rooms available.\n";
            return;
        }
        int count = 1;
        for (const auto& pair : rooms) {
            cout << count++ << ". " << pair.first << "\n";
        }
        cout << "Total number of rooms: " << rooms.size() << "\n";
    }

    void bookMeeting() {
        if (rooms.empty()) {
            cout << "[ERROR] No rooms available. Please add a room first.\n";
            return;
        }
        cout << "Room name: ";
        string roomName;
        getline(cin, roomName);
        
        if (rooms.find(roomName) == rooms.end()) {
            cout << "[ERROR] Room not found.\n";
            return;
        }

        cout << "Start time (HH:MM): ";
        string startStr; getline(cin, startStr);
        int start = timeToMins(startStr);

        cout << "End time (HH:MM): ";
        string endStr; getline(cin, endStr);
        int end = timeToMins(endStr);

        if (start == -1 || end == -1 || start >= end) {
            cout << "[ERROR] Invalid time format or range.\n";
            return;
        }

        // Conflict check before booking
        vector<Interval> conflicts = rooms[roomName].getAllConflicts(Interval(start, end));
        if (!conflicts.empty()) {
            cout << "\n[ERROR] Overlap detected with existing meetings:\n";
            for (const auto& c : conflicts) {
                string t, d;
                decodeLabel(c.label, t, d);
                cout << "   -> [" << minsToTime(c.start) << " to " << minsToTime(c.end) << "] " << t << "\n";
            }
            return;
        }

        cout << "Meeting Title: ";
        string title; getline(cin, title);
        
        cout << "Meeting Description: ";
        string desc; getline(cin, desc);

        rooms[roomName].insert(Interval(start, end, encodeLabel(title, desc)));
        cout << "[SUCCESS] Meeting booked successfully!\n";
    }

    void checkAvailability() {
        cout << "Enter Room Name: ";
        string rName; getline(cin, rName);
        if (rooms.find(rName) == rooms.end()) {
            cout << "[ERROR] Room does not exist.\n";
            return;
        }
        cout << "Start Time (HH:MM): ";
        string startStr; getline(cin, startStr);
        int st = timeToMins(startStr);

        cout << "End Time (HH:MM): ";
        string endStr; getline(cin, endStr);
        int en = timeToMins(endStr);

        if (st == -1 || en == -1 || st >= en) {
            cout << "[ERROR] Invalid times.\n";
            return;
        }

        vector<Interval> confs = rooms[rName].getAllConflicts(Interval(st, en));
        if (confs.empty()) {
            cout << "[SUCCESS] Room is available for this time range.\n";
        } else {
            cout << "[INFO] Room is NOT available. Conflicting meetings:\n";
            for (const auto& c : confs) {
                string t, d; decodeLabel(c.label, t, d);
                cout << "   -> [" << minsToTime(c.start) << " to " << minsToTime(c.end) << "] " << t << "\n";
            }
        }
    }

    void manageBookings() {
        currentBookingsView.clear();
        for (auto& pair : rooms) {
            // Passing 0 to 1440 (24 hours) fetches all intervals for the day
            vector<Interval> ivs = pair.second.getAllConflicts(Interval(0, 1440));
            for (auto& iv : ivs) {
                currentBookingsView.push_back({pair.first, iv});
            }
        }

        cout << "\n=== All Bookings ===\n";
        if (currentBookingsView.empty()) {
            cout << "No meetings currently booked.\n";
            return;
        }

        cout << left << setw(4) << "ID" << " | "
             << left << setw(12) << "Room" << " | "
             << left << setw(5) << "Start" << " | "
             << left << setw(5) << "End" << " | "
             << left << setw(20) << "Title" << " | "
             << "Description\n";
        cout << string(75, '=') << "\n";

        for (size_t i = 0; i < currentBookingsView.size(); ++i) {
            string t, d;
            decodeLabel(currentBookingsView[i].second.label, t, d);
            cout << left << setw(4) << (i + 1) << " | "
                 << left << setw(12) << currentBookingsView[i].first << " | "
                 << left << setw(5) << minsToTime(currentBookingsView[i].second.start) << " | "
                 << left << setw(5) << minsToTime(currentBookingsView[i].second.end) << " | "
                 << left << setw(20) << t.substr(0, 19) << " | "
                 << d << "\n";
        }

        cout << "\nEnter Booking ID to manage (or 0 to return): ";
        int targetId;
        if (!(cin >> targetId)) {
            cin.clear(); cin.ignore(10000, '\n'); return;
        }
        cin.ignore(10000, '\n');

        if (targetId == 0) return;
        
        if (targetId < 1 || targetId > (int)currentBookingsView.size()) {
            cout << "[ERROR] Invalid ID.\n";
            return;
        }

        processUpdateOrCancel(targetId);
    }

    void processUpdateOrCancel(int id) {
        auto& target = currentBookingsView[id - 1];
        string roomName = target.first;
        Interval oldIv = target.second;

        string t, d;
        decodeLabel(oldIv.label, t, d);

        cout << "\nSelected Booking Details:\n";
        cout << "Room: " << roomName << "\n"
             << "Time: " << minsToTime(oldIv.start) << " to " << minsToTime(oldIv.end) << "\n"
             << "Title: " << t << "\n\n";

        cout << "1. Update Booking\n";
        cout << "2. Cancel Booking\n";
        cout << "3. Go Back\n";
        cout << "Action: ";
        
        int action;
        if (!(cin >> action)) {
            cin.clear(); cin.ignore(10000, '\n'); return;
        }
        cin.ignore(10000, '\n');

        if (action == 2) {
            rooms[roomName].remove(oldIv.start, oldIv.end);
            cout << "[SUCCESS] Booking canceled successfully.\n";
        } else if (action == 1) {
            cout << "Enter new Start Time (HH:MM): ";
            string nStartStr; getline(cin, nStartStr);
            int nStart = timeToMins(nStartStr);

            cout << "Enter new End Time (HH:MM): ";
            string nEndStr; getline(cin, nEndStr);
            int nEnd = timeToMins(nEndStr);

            if (nStart == -1 || nEnd == -1 || nStart >= nEnd) {
                cout << "[ERROR] Invalid time format.\n";
                return;
            }

            cout << "Enter new Title: ";
            string nTitle; getline(cin, nTitle);
            cout << "Enter new Description: ";
            string nDesc; getline(cin, nDesc);

            // Remove temporarily to verify conflicts for the new time
            rooms[roomName].remove(oldIv.start, oldIv.end);

            Interval newIv(nStart, nEnd, encodeLabel(nTitle, nDesc));
            vector<Interval> confs = rooms[roomName].getAllConflicts(newIv);

            if (!confs.empty()) {
                cout << "[ERROR] Update failed due to time conflict! Reverting to original booking.\n";
                rooms[roomName].insert(oldIv); 
            } else {
                rooms[roomName].insert(newIv);
                cout << "[SUCCESS] Booking updated successfully.\n";
            }
        }
    }

    void printTreeForRoom() {
        cout << "Enter Room Name: ";
        string rName; getline(cin, rName);
        if (rooms.find(rName) == rooms.end()) {
            cout << "[ERROR] Room does not exist.\n";
            return;
        }

        cout << "\n=== Interval Tree for " << rName << " ===\n";
        Node* root = rooms[rName].getRoot();
        if (!root) {
            cout << "(Tree is empty)\n";
        } else {
            printNodeTree(root, 0);
        }
        cout << "\n";
    }
};

int main() {
    MeetingScheduler app;
    int choice;

    while (true) {
        cout << "\n=========================================\n";
        cout << "        MEETING ROOM SCHEDULER\n";
        cout << "=========================================\n";
        cout << "1. Add new room\n";
        cout << "2. View all rooms\n";
        cout << "3. Book a meeting\n";
        cout << "4. Check room availability\n";
        cout << "5. View & Manage bookings (Table View)\n";
        cout << "6. View room Interval Tree\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Invalid input.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        if (choice == 1) app.addRoom();
        else if (choice == 2) app.viewRooms();
        else if (choice == 3) app.bookMeeting();
        else if (choice == 4) app.checkAvailability();
        else if (choice == 5) app.manageBookings();
        else if (choice == 6) app.printTreeForRoom();
        else if (choice == 7) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "[ERROR] Invalid choice, try again.\n";
        }
    }

    return 0;
}