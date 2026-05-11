#ifndef SCHEDULER_SERVICE_H
#define SCHEDULER_SERVICE_H

#include <QString>
#include <QVector>
#include <memory>
#include <vector>

#include "intervalTree.h"

struct Booking {
    int start;
    int end;
    QString title;
    QString description;
};

struct OperationResult {
    bool ok = false;
    bool free = false;
    QString message;
    QVector<Booking> conflicts;
};

class SchedulerService
{
public:
    SchedulerService();

    bool addRoom(const QString& roomName, QString& message);

    OperationResult bookMeeting(
        const QString& roomName,
        int start,
        int end,
        const QString& title,
        const QString& description
    );

    OperationResult checkAvailability(
        const QString& roomName,
        int start,
        int end
    );

    OperationResult cancelBooking(
        const QString& roomName,
        int bookingIndex
    );

    OperationResult updateBooking(
        const QString& roomName,
        int bookingIndex,
        int newStart,
        int newEnd,
        const QString& newTitle,
        const QString& newDescription
    );

    QStringList getRooms() const;
    QVector<Booking> getBookings(const QString& roomName) const;
    QVector<Booking> getAllBookingsWithRoom(QStringList& roomNamesOut) const;

private:
    struct RoomData {
        QString name;
        std::unique_ptr<IntervalTree> tree;
        QVector<Booking> bookings;

        RoomData(const QString& roomName);

        RoomData(RoomData&&) noexcept = default;
        RoomData& operator=(RoomData&&) noexcept = default;

        RoomData(const RoomData&) = delete;
        RoomData& operator=(const RoomData&) = delete;
    };

    std::vector<RoomData> rooms;

    int findRoomIndex(const QString& roomName) const;
    void sortBookings(RoomData& room);
    QVector<Booking> convertConflicts(const std::vector<Interval>& intervals) const;
};

#endif