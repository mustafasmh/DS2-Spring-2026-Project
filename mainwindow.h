#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QTimeEdit>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>

#include "scheduler_service.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    SchedulerService scheduler;

    QListWidget* roomListWidget;

    QLineEdit* roomNameInput;

    QComboBox* bookingRoomCombo;
    QTimeEdit* startTimeEdit;
    QTimeEdit* endTimeEdit;
    QLineEdit* titleInput;
    QTextEdit* descriptionInput;

    QComboBox* checkRoomCombo;
    QTimeEdit* checkStartTimeEdit;
    QTimeEdit* checkEndTimeEdit;

    QTableWidget* bookingTable;

    QComboBox* treeRoomCombo;
    QTextEdit* treeText;

    QLabel* statusLabel;
    QLabel* roomCountLabel;
    QLabel* bookingCountLabel;

    void buildUi();
    void applyStyle();

    void refreshAll();
    void refreshRooms();
    void refreshDropdowns();
    void refreshStats();
    void refreshBookingTable();
    void refreshTreeText();

    void addRoom();
    void bookMeeting();
    void checkAvailability();
    void updateSelectedBooking();
    void cancelSelectedBooking();

    int qtimeToMinutes(const QTime& time) const;
    QTime minutesToQTime(int minutes) const;
    QString formatTime(int minutes) const;

    void showStatus(const QString& message, const QString& type);
    QString conflictsToText(const QVector<Booking>& conflicts) const;

    QString buildTreeText(const QVector<Booking>& bookings);
    QString renderTreeNode(const QVector<Booking>& bookings, int left, int right, int depth);
    int maxEndInRange(const QVector<Booking>& bookings, int left, int right);
};

#endif