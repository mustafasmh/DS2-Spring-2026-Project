#include <iostream>
#include <string>
#include "scheduler.h"
using namespace std;

void printMenu() {
    cout << "\n===== Meeting Room Scheduler =====\n";
    cout << "1. Add room\n";
    cout << "2. Book meeting\n";
    cout << "3. Cancel meeting\n";
    cout << "4. Update meeting\n";
    cout << "5. Check availability\n";
    cout << "6. View room schedule\n";
    cout << "7. View all rooms\n";
    cout << "8. Exit\n";
    cout << "Choice: ";
}

int main() {
    MeetingScheduler scheduler;
    int choice;

    while (true) {
        printMenu();
        cin >> choice;
        cin.ignore();  // flush newline after number input

        if (choice == 1) {
            string name;
            cout << "Room name: ";
            getline(cin, name);
            scheduler.addRoom(name);

        } else if (choice == 2) {
            string room, start, end, label;
            cout << "Room name: ";
            getline(cin, room);
            cout << "Start time (HH:MM): ";
            getline(cin, start);
            cout << "End time (HH:MM): ";
            getline(cin, end);
            cout << "Meeting label: ";
            getline(cin, label);
            scheduler.book(room, start, end, label);

        } else if (choice == 3) {
            string room, start, end;
            cout << "Room name: ";
            getline(cin, room);
            cout << "Start time (HH:MM): ";
            getline(cin, start);
            cout << "End time (HH:MM): ";
            getline(cin, end);
            scheduler.cancel(room, start, end);

        } else if (choice == 4) {
            string room, oldStart, oldEnd, newStart, newEnd, label;
            cout << "Room name: ";
            getline(cin, room);
            cout << "Old start time (HH:MM): ";
            getline(cin, oldStart);
            cout << "Old end time (HH:MM): ";
            getline(cin, oldEnd);
            cout << "New start time (HH:MM): ";
            getline(cin, newStart);
            cout << "New end time (HH:MM): ";
            getline(cin, newEnd);
            cout << "New label: ";
            getline(cin, label);
            scheduler.update(room, oldStart, oldEnd, newStart, newEnd, label);

        } else if (choice == 5) {
            string room, start, end;
            cout << "Room name: ";
            getline(cin, room);
            cout << "Start time (HH:MM): ";
            getline(cin, start);
            cout << "End time (HH:MM): ";
            getline(cin, end);
            scheduler.checkAvailability(room, start, end);

        } else if (choice == 6) {
            string room;
            cout << "Room name: ";
            getline(cin, room);
            scheduler.printRoom(room);

        } else if (choice == 7) {
            scheduler.printAll();

        } else if (choice == 8) {
            cout << "Goodbye!\n";
            break;

        } else {
            cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}