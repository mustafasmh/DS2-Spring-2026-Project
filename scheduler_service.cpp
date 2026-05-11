#include "scheduler_service.h"

#include <algorithm>

static std::string toStd(const QString& value)
{
    return value.toStdString();
}

SchedulerService::RoomData::RoomData(const QString& roomName)
{
    name = roomName;
    tree = std::make_unique<IntervalTree>();
}

SchedulerService::SchedulerService()
{
    QString msg;

    addRoom("Room A", msg);
    addRoom("Room B", msg);
    addRoom("Conference Hall", msg);

    bookMeeting(
        "Room A",
        9 * 60,
        10 * 60,
        "Team Meeting",
        "Discussion on project progress and pending tasks."
    );

    bookMeeting(
        "Room A",
        11 * 60,
        12 * 60,
        "Interview",
        "Candidate interview with faculty panel."
    );

    bookMeeting(
        "Room B",
        10 * 60 + 30,
        11 * 60 + 30,
        "Presentation",
        "Student project presentation and review."
    );
}

int SchedulerService::findRoomIndex(const QString& roomName) const
{
    for (int i = 0; i < static_cast<int>(rooms.size()); i++) {
        if (rooms[i].name.compare(roomName, Qt::CaseInsensitive) == 0) {
            return i;
        }
    }

    return -1;
}

bool SchedulerService::addRoom(const QString& roomName, QString& message)
{
    QString cleanName = roomName.trimmed();

    if (cleanName.isEmpty()) {
        message = "Room name is required.";
        return false;
    }

    if (findRoomIndex(cleanName) != -1) {
        message = "Room already exists.";
        return false;
    }

    rooms.emplace_back(cleanName);
    message = "Room added successfully.";
    return true;
}

OperationResult SchedulerService::bookMeeting(
    const QString& roomName,
    int start,
    int end,
    const QString& title,
    const QString& description
) {
    OperationResult result;

    int idx = findRoomIndex(roomName);

    if (idx == -1) {
        result.message = "Room not found.";
        return result;
    }

    if (title.trimmed().isEmpty()) {
        result.message = "Meeting title is required.";
        return result;
    }

    if (start >= end) {
        result.message = "End time must be after start time.";
        return result;
    }

    Interval newSlot(start, end, toStd(title));
    std::vector<Interval> conflicts = rooms[idx].tree->getAllConflicts(newSlot);

    if (!conflicts.empty()) {
        result.ok = false;
        result.message = "Conflict detected. Meeting cannot be booked.";
        result.conflicts = convertConflicts(conflicts);
        return result;
    }

    rooms[idx].tree->insert(newSlot);

    Booking booking;
    booking.start = start;
    booking.end = end;
    booking.title = title.trimmed();
    booking.description = description.trimmed();

    rooms[idx].bookings.push_back(booking);
    sortBookings(rooms[idx]);

    result.ok = true;
    result.message = "Meeting booked successfully.";
    return result;
}

OperationResult SchedulerService::checkAvailability(
    const QString& roomName,
    int start,
    int end
) {
    OperationResult result;

    int idx = findRoomIndex(roomName);

    if (idx == -1) {
        result.message = "Room not found.";
        return result;
    }

    if (start >= end) {
        result.message = "End time must be after start time.";
        return result;
    }

    Interval query(start, end);
    std::vector<Interval> conflicts = rooms[idx].tree->getAllConflicts(query);

    result.ok = true;

    if (conflicts.empty()) {
        result.free = true;
        result.message = "Room is free.";
        return result;
    }

    result.free = false;
    result.message = "Room is busy.";
    result.conflicts = convertConflicts(conflicts);
    return result;
}

OperationResult SchedulerService::cancelBooking(
    const QString& roomName,
    int bookingIndex
) {
    OperationResult result;

    int idx = findRoomIndex(roomName);

    if (idx == -1) {
        result.message = "Room not found.";
        return result;
    }

    if (bookingIndex < 0 || bookingIndex >= rooms[idx].bookings.size()) {
        result.message = "Booking not found.";
        return result;
    }

    Booking oldBooking = rooms[idx].bookings[bookingIndex];

    rooms[idx].tree->remove(oldBooking.start, oldBooking.end);
    rooms[idx].bookings.removeAt(bookingIndex);

    result.ok = true;
    result.message = "Booking cancelled successfully.";
    return result;
}

OperationResult SchedulerService::updateBooking(
    const QString& roomName,
    int bookingIndex,
    int newStart,
    int newEnd,
    const QString& newTitle,
    const QString& newDescription
) {
    OperationResult result;

    int idx = findRoomIndex(roomName);

    if (idx == -1) {
        result.message = "Room not found.";
        return result;
    }

    if (bookingIndex < 0 || bookingIndex >= rooms[idx].bookings.size()) {
        result.message = "Booking not found.";
        return result;
    }

    if (newTitle.trimmed().isEmpty()) {
        result.message = "Meeting title is required.";
        return result;
    }

    if (newStart >= newEnd) {
        result.message = "End time must be after start time.";
        return result;
    }

    Booking oldBooking = rooms[idx].bookings[bookingIndex];

    rooms[idx].tree->remove(oldBooking.start, oldBooking.end);

    Interval newSlot(newStart, newEnd, toStd(newTitle));
    std::vector<Interval> conflicts = rooms[idx].tree->getAllConflicts(newSlot);

    if (!conflicts.empty()) {
        Interval oldSlot(oldBooking.start, oldBooking.end, toStd(oldBooking.title));
        rooms[idx].tree->insert(oldSlot);

        result.ok = false;
        result.message = "Conflict detected. Booking cannot be updated.";
        result.conflicts = convertConflicts(conflicts);
        return result;
    }

    rooms[idx].tree->insert(newSlot);

    rooms[idx].bookings[bookingIndex] = {
        newStart,
        newEnd,
        newTitle.trimmed(),
        newDescription.trimmed()
    };

    sortBookings(rooms[idx]);

    result.ok = true;
    result.message = "Booking updated successfully.";
    return result;
}

QStringList SchedulerService::getRooms() const
{
    QStringList names;

    for (const auto& room : rooms) {
        names << room.name;
    }

    return names;
}

QVector<Booking> SchedulerService::getBookings(const QString& roomName) const
{
    int idx = findRoomIndex(roomName);

    if (idx == -1) {
        return {};
    }

    return rooms[idx].bookings;
}

QVector<Booking> SchedulerService::getAllBookingsWithRoom(QStringList& roomNamesOut) const
{
    QVector<Booking> all;
    roomNamesOut.clear();

    for (const auto& room : rooms) {
        for (const auto& booking : room.bookings) {
            all.push_back(booking);
            roomNamesOut.push_back(room.name);
        }
    }

    return all;
}

void SchedulerService::sortBookings(RoomData& room)
{
    std::sort(room.bookings.begin(), room.bookings.end(), [](const Booking& a, const Booking& b) {
        return a.start < b.start;
    });
}

QVector<Booking> SchedulerService::convertConflicts(const std::vector<Interval>& intervals) const
{
    QVector<Booking> converted;

    for (const Interval& interval : intervals) {
        Booking booking;
        booking.start = interval.start;
        booking.end = interval.end;
        booking.title = QString::fromStdString(interval.label);
        booking.description = "";
        converted.push_back(booking);
    }

    return converted;
}