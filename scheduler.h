#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <string>
#include <intervalTree.h*>

using namespace std;

class MeetingScheduler {

    private:
        vector<string> roomNames;
        vector<IntervalTree*> rooms;

        // Convert "HH:MM" to minutes from midnight
        int toMinutes(const string& t) {
            int h = stoi(t.substr(0, 2));
            int m = stoi(t.substr(3, 2));
            return h * 60 + m;
        }

        // Convert minutes back to "HH:MM"
        string toTime(int minutes) {
            int h = minutes / 60;
            int m = minutes % 60;

            return (h < 10 ? "0" : "") + to_string(h) + ":" +
                   (m < 10 ? "0" : "") + to_string(m);
        }

        // Find index of a room by name, returns -1 if not found
        int findRoom(const string& name) {
            for (int i = 0; i < roomNames.size(); i++)
                if (roomNames[i] == name) return i;
            return -1;
        }
    
    public:
        MeetingScheduler() {}
        
        ~MeetingScheduler() {
            for (int i = 0; i < rooms.size(); i++)
                delete rooms[i];
        }

        void addRoom(const string& name) {
            if (findRoom(name) != -1) {
                cout << "Room \"" << name << "\" already exists. \n";
                return;
            }

            roomNames.push_back(name);
            rooms.push_back(new IntervalTree());
            cout << "Room \"" << name << "\" added. \n";
        }

        void book(const string& room, const string& start, const string& end, const string& label) {
            int idx = findRoom(room);
            if (idx == -1){
                cout << "Room \"" << room << "\" not found. \n";
                return;
            }

            Interval newSlot(toMinutes(start), toMinutes(end), label);
            vector<Interval> conflicts = rooms[idx]->getAllConflicts(newSlot);

            if (!conflicts.empty()) {
                cout << "CONFLICT! Cannot book \"" << label << "\" in" << room << ".\n";
                cout << "Clashes with:\n";
                for (int i = 0; i < conflicts.size(); i++)
                    cout << "  " << toTime(conflicts[i].start)
                         << " - " << toTime(conflicts[i].end)
                         << " " << conflicts[i].label << "\n";
                return;
            }

            rooms[idx]->insert(newSlot);
            cout << "Booked \"" << label << "\" in " << room
                 << " from " << start << " to " << end << "\n";

        }

        void cancel(const string& room, const string& start, const string& end) {
            int idx = findRoom(room);
            if (idx == -1){
                cout << "Room \"" << room << "\" not found. \n";
                return;
            }

            bool removed = rooms[idx]->remove(toMinutes(start), toMinutes(end));
            if (removed)
                cout << "Cancelled booking " << start << " - " << end
                     << " in " << room << "\n";
            else 
                cout << "No booking found at " << start << " - " << end
                     << " in " << room << "\n";
        }

        void update(const string& room, const string& oldStart, const string& oldEnd,
                    const string& newStart, const string& newEnd, const string& newLabel) {
            int idx = findRoom(room);
            if (idx == -1) {
                cout << "Room \"" << room << "\" not found.\n";
                return;
            }
        
            // Step 1: remove the old booking
            bool removed = rooms[idx]->remove(toMinutes(oldStart), toMinutes(oldEnd));
            if (!removed) {
                cout << "No booking found at " << oldStart << " - " << oldEnd
                     << " in " << room << "\n";
                return;
            }
        
            // Step 2: check if new slot conflicts with anything
            Interval newSlot(toMinutes(newStart), toMinutes(newEnd), newLabel);
            vector<Interval> conflicts = rooms[idx]->getAllConflicts(newSlot);
        
            if (!conflicts.empty()) {
                // Conflict found — reinsert the old booking to keep tree consistent
                rooms[idx]->insert(Interval(toMinutes(oldStart), toMinutes(oldEnd), newLabel));
                cout << "CONFLICT! Cannot update booking in " << room << ".\n";
                cout << "Clashes with:\n";
                for (int i = 0; i < conflicts.size(); i++)
                    cout << "  " << toTime(conflicts[i].start)
                         << " - " << toTime(conflicts[i].end)
                         << " " << conflicts[i].label << "\n";
                return;
            }
        
            // Step 3: insert the new booking
            rooms[idx]->insert(newSlot);
            cout << "Updated booking in " << room << " to "
                 << newStart << " - " << newEnd << " \"" << newLabel << "\"\n";
        }

        void checkAvailability(const string& room, const string& start, const string& end) {
            int idx = findRoom(room);
            if (idx == -1) {
                cout << "Room \"" << room << "\" not found.\n";
                return;
            }
        
            Interval query(toMinutes(start), toMinutes(end));
            vector<Interval> conflicts = rooms[idx]->getAllConflicts(query);
        
            if (conflicts.empty()) {
                cout << room << " is FREE from " << start << " to " << end << "\n";
            } else {
                cout << room << " is BUSY from " << start << " to " << end << ".\n";
                cout << "Conflicting bookings:\n";
                for (int i = 0; i < conflicts.size(); i++)
                    cout << "  " << toTime(conflicts[i].start)
                         << " - " << toTime(conflicts[i].end)
                         << " " << conflicts[i].label << "\n";
            }
        }

};

#endif