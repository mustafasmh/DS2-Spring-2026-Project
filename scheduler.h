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

};

#endif