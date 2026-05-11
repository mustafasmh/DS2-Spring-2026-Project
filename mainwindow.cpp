#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
    applyStyle();
    refreshAll();

    setWindowTitle("Meeting Scheduler - C++ Interval Tree");
    resize(1100, 750);
}

void MainWindow::buildUi()
{
    QWidget* page = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(18);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);
    setCentralWidget(scrollArea);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout;

    QVBoxLayout* titleLayout = new QVBoxLayout;
    QLabel* title = new QLabel("Meeting Scheduler");
    title->setObjectName("titleLabel");

    QLabel* subtitle = new QLabel("Simple C++ GUI using Interval Tree conflict detection.");
    subtitle->setObjectName("subtitleLabel");

    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);

    QHBoxLayout* statsLayout = new QHBoxLayout;

    roomCountLabel = new QLabel("Rooms: 0");
    roomCountLabel->setObjectName("statLabel");

    bookingCountLabel = new QLabel("Bookings: 0");
    bookingCountLabel->setObjectName("statLabel");

    statsLayout->addWidget(roomCountLabel);
    statsLayout->addWidget(bookingCountLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(statsLayout);

    mainLayout->addLayout(headerLayout);

    // Status
    statusLabel = new QLabel("Ready.");
    statusLabel->setObjectName("statusInfo");
    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    // Top section
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setSpacing(18);

    // Rooms group
    QGroupBox* roomsGroup = new QGroupBox("Rooms");
    QVBoxLayout* roomsLayout = new QVBoxLayout(roomsGroup);

    QHBoxLayout* addRoomLayout = new QHBoxLayout;
    roomNameInput = new QLineEdit;
    roomNameInput->setPlaceholderText("Enter room name");

    QPushButton* addRoomButton = new QPushButton("Add Room");

    addRoomLayout->addWidget(roomNameInput);
    addRoomLayout->addWidget(addRoomButton);

    roomListWidget = new QListWidget;

    roomsLayout->addLayout(addRoomLayout);
    roomsLayout->addWidget(roomListWidget);

    connect(addRoomButton, &QPushButton::clicked, this, &MainWindow::addRoom);

    // Book group
    QGroupBox* bookGroup = new QGroupBox("Book Meeting");
    QGridLayout* bookLayout = new QGridLayout(bookGroup);

    bookingRoomCombo = new QComboBox;

    startTimeEdit = new QTimeEdit;
    endTimeEdit = new QTimeEdit;

    startTimeEdit->setDisplayFormat("HH:mm");
    endTimeEdit->setDisplayFormat("HH:mm");

    startTimeEdit->setTime(QTime(9, 0));
    endTimeEdit->setTime(QTime(10, 0));

    titleInput = new QLineEdit;
    titleInput->setPlaceholderText("Meeting title");

    descriptionInput = new QTextEdit;
    descriptionInput->setPlaceholderText("Optional description");
    descriptionInput->setFixedHeight(70);

    QPushButton* bookButton = new QPushButton("Book Meeting");
    bookButton->setObjectName("successButton");

    bookLayout->addWidget(new QLabel("Room:"), 0, 0);
    bookLayout->addWidget(bookingRoomCombo, 0, 1);

    bookLayout->addWidget(new QLabel("Start:"), 1, 0);
    bookLayout->addWidget(startTimeEdit, 1, 1);

    bookLayout->addWidget(new QLabel("End:"), 2, 0);
    bookLayout->addWidget(endTimeEdit, 2, 1);

    bookLayout->addWidget(new QLabel("Title:"), 3, 0);
    bookLayout->addWidget(titleInput, 3, 1);

    bookLayout->addWidget(new QLabel("Description:"), 4, 0);
    bookLayout->addWidget(descriptionInput, 4, 1);

    bookLayout->addWidget(bookButton, 5, 1);

    connect(bookButton, &QPushButton::clicked, this, &MainWindow::bookMeeting);

    topLayout->addWidget(roomsGroup, 1);
    topLayout->addWidget(bookGroup, 2);

    mainLayout->addLayout(topLayout);

    // Availability group
    QGroupBox* checkGroup = new QGroupBox("Check Availability");
    QGridLayout* checkLayout = new QGridLayout(checkGroup);

    checkRoomCombo = new QComboBox;

    checkStartTimeEdit = new QTimeEdit;
    checkEndTimeEdit = new QTimeEdit;

    checkStartTimeEdit->setDisplayFormat("HH:mm");
    checkEndTimeEdit->setDisplayFormat("HH:mm");

    checkStartTimeEdit->setTime(QTime(10, 0));
    checkEndTimeEdit->setTime(QTime(11, 0));

    QPushButton* checkButton = new QPushButton("Check");

    checkLayout->addWidget(new QLabel("Room:"), 0, 0);
    checkLayout->addWidget(checkRoomCombo, 0, 1);

    checkLayout->addWidget(new QLabel("Start:"), 0, 2);
    checkLayout->addWidget(checkStartTimeEdit, 0, 3);

    checkLayout->addWidget(new QLabel("End:"), 0, 4);
    checkLayout->addWidget(checkEndTimeEdit, 0, 5);

    checkLayout->addWidget(checkButton, 0, 6);

    connect(checkButton, &QPushButton::clicked, this, &MainWindow::checkAvailability);

    mainLayout->addWidget(checkGroup);

    // Booking table
    QGroupBox* tableGroup = new QGroupBox("All Bookings");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);

    bookingTable = new QTableWidget;
    bookingTable->setColumnCount(5);
    bookingTable->setHorizontalHeaderLabels({ "Room", "Start", "End", "Title", "Description" });
    bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bookingTable->setMinimumHeight(260);

    QHBoxLayout* tableButtonLayout = new QHBoxLayout;

    QPushButton* updateButton = new QPushButton("Update Selected");
    updateButton->setObjectName("warningButton");

    QPushButton* cancelButton = new QPushButton("Cancel Selected");
    cancelButton->setObjectName("dangerButton");

    tableButtonLayout->addWidget(updateButton);
    tableButtonLayout->addWidget(cancelButton);
    tableButtonLayout->addStretch();

    connect(updateButton, &QPushButton::clicked, this, &MainWindow::updateSelectedBooking);
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::cancelSelectedBooking);

    tableLayout->addWidget(bookingTable);
    tableLayout->addLayout(tableButtonLayout);

    mainLayout->addWidget(tableGroup);

    // Simple tree text
    QGroupBox* treeGroup = new QGroupBox("Simple Interval Tree View");
    QVBoxLayout* treeLayout = new QVBoxLayout(treeGroup);

    treeRoomCombo = new QComboBox;

    treeText = new QTextEdit;
    treeText->setReadOnly(true);
    treeText->setMinimumHeight(240);
    treeText->setFontFamily("Consolas");

    QLabel* treeNote = new QLabel(
        "This is a simple visual representation. Conflict detection is still done by your actual C++ IntervalTree class."
    );
    treeNote->setWordWrap(true);
    treeNote->setObjectName("noteLabel");

    treeLayout->addWidget(new QLabel("Select room:"));
    treeLayout->addWidget(treeRoomCombo);
    treeLayout->addWidget(treeNote);
    treeLayout->addWidget(treeText);

    connect(treeRoomCombo, &QComboBox::currentTextChanged, this, &MainWindow::refreshTreeText);

    mainLayout->addWidget(treeGroup);
}

void MainWindow::applyStyle()
{
    setStyleSheet(R"(
        QWidget {
            font-family: "Segoe UI";
            font-size: 14px;
            background: #f5f7fb;
            color: #172033;
        }

        QScrollArea {
            border: none;
            background: #f5f7fb;
        }

        #titleLabel {
            font-size: 34px;
            font-weight: 900;
            background: transparent;
        }

        #subtitleLabel {
            color: #64748b;
            font-size: 15px;
            background: transparent;
        }

        #statLabel {
            background: #ffffff;
            border: 1px solid #e5eaf2;
            border-radius: 12px;
            padding: 12px 18px;
            font-weight: 800;
        }

        #statusInfo {
            background: #eff6ff;
            color: #1d4ed8;
            border: 1px solid #bfdbfe;
            border-radius: 12px;
            padding: 12px;
        }

        #statusSuccess {
            background: #ecfdf3;
            color: #166534;
            border: 1px solid #bbf7d0;
            border-radius: 12px;
            padding: 12px;
        }

        #statusError {
            background: #fef2f2;
            color: #991b1b;
            border: 1px solid #fecaca;
            border-radius: 12px;
            padding: 12px;
        }

        QGroupBox {
            background: #ffffff;
            border: 1px solid #e5eaf2;
            border-radius: 14px;
            margin-top: 12px;
            padding: 16px;
            font-size: 16px;
            font-weight: 800;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
            background: #ffffff;
        }

        QLineEdit, QComboBox, QTimeEdit, QTextEdit, QListWidget, QTableWidget {
            background: #ffffff;
            border: 1px solid #dbe4f0;
            border-radius: 8px;
            padding: 6px;
        }

        QPushButton {
            background: #2563eb;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 9px 14px;
            font-weight: 800;
        }

        QPushButton:hover {
            background: #1d4ed8;
        }

        #successButton {
            background: #16a34a;
        }

        #successButton:hover {
            background: #15803d;
        }

        #warningButton {
            background: #d97706;
        }

        #warningButton:hover {
            background: #b45309;
        }

        #dangerButton {
            background: #dc2626;
        }

        #dangerButton:hover {
            background: #b91c1c;
        }

        QHeaderView::section {
            background: #f8fafc;
            padding: 8px;
            border: none;
            border-bottom: 1px solid #e5eaf2;
            font-weight: 800;
        }

        #noteLabel {
            color: #64748b;
            background: #f8fafc;
            border: 1px solid #e5eaf2;
            border-radius: 8px;
            padding: 8px;
        }
    )");
}

void MainWindow::refreshAll()
{
    refreshRooms();
    refreshDropdowns();
    refreshStats();
    refreshBookingTable();
    refreshTreeText();
}

void MainWindow::refreshRooms()
{
    roomListWidget->clear();

    QStringList rooms = scheduler.getRooms();

    for (const QString& room : rooms) {
        roomListWidget->addItem(room);
    }
}

void MainWindow::refreshDropdowns()
{
    QString selectedBookingRoom = bookingRoomCombo->currentText();
    QString selectedCheckRoom = checkRoomCombo->currentText();
    QString selectedTreeRoom = treeRoomCombo->currentText();

    QStringList rooms = scheduler.getRooms();

    bookingRoomCombo->clear();
    checkRoomCombo->clear();
    treeRoomCombo->clear();

    bookingRoomCombo->addItems(rooms);
    checkRoomCombo->addItems(rooms);
    treeRoomCombo->addItems(rooms);

    if (rooms.contains(selectedBookingRoom)) {
        bookingRoomCombo->setCurrentText(selectedBookingRoom);
    }

    if (rooms.contains(selectedCheckRoom)) {
        checkRoomCombo->setCurrentText(selectedCheckRoom);
    }

    if (rooms.contains(selectedTreeRoom)) {
        treeRoomCombo->setCurrentText(selectedTreeRoom);
    }
}

void MainWindow::refreshStats()
{
    QStringList roomNames;
    QVector<Booking> bookings = scheduler.getAllBookingsWithRoom(roomNames);

    roomCountLabel->setText("Rooms: " + QString::number(scheduler.getRooms().size()));
    bookingCountLabel->setText("Bookings: " + QString::number(bookings.size()));
}

void MainWindow::refreshBookingTable()
{
    bookingTable->setRowCount(0);

    QStringList rooms = scheduler.getRooms();

    int row = 0;

    for (const QString& roomName : rooms) {
        QVector<Booking> bookings = scheduler.getBookings(roomName);

        for (int bookingIndex = 0; bookingIndex < bookings.size(); bookingIndex++) {
            const Booking& booking = bookings[bookingIndex];

            bookingTable->insertRow(row);

            QTableWidgetItem* roomItem = new QTableWidgetItem(roomName);
            QTableWidgetItem* startItem = new QTableWidgetItem(formatTime(booking.start));
            QTableWidgetItem* endItem = new QTableWidgetItem(formatTime(booking.end));
            QTableWidgetItem* titleItem = new QTableWidgetItem(booking.title);
            QTableWidgetItem* descItem = new QTableWidgetItem(booking.description);

            roomItem->setData(Qt::UserRole, roomName);
            roomItem->setData(Qt::UserRole + 1, bookingIndex);

            bookingTable->setItem(row, 0, roomItem);
            bookingTable->setItem(row, 1, startItem);
            bookingTable->setItem(row, 2, endItem);
            bookingTable->setItem(row, 3, titleItem);
            bookingTable->setItem(row, 4, descItem);

            row++;
        }
    }
}

void MainWindow::refreshTreeText()
{
    QString roomName = treeRoomCombo->currentText();

    if (roomName.isEmpty()) {
        treeText->setText("No room selected.");
        return;
    }

    QVector<Booking> bookings = scheduler.getBookings(roomName);

    if (bookings.isEmpty()) {
        treeText->setText("No bookings in this room.");
        return;
    }

    treeText->setText(buildTreeText(bookings));
}

void MainWindow::addRoom()
{
    QString message;
    bool ok = scheduler.addRoom(roomNameInput->text(), message);

    if (!ok) {
        showStatus(message, "error");
        return;
    }

    roomNameInput->clear();
    showStatus(message, "success");
    refreshAll();
}

void MainWindow::bookMeeting()
{
    OperationResult result = scheduler.bookMeeting(
        bookingRoomCombo->currentText(),
        qtimeToMinutes(startTimeEdit->time()),
        qtimeToMinutes(endTimeEdit->time()),
        titleInput->text(),
        descriptionInput->toPlainText()
    );

    if (!result.ok) {
        showStatus(result.message + conflictsToText(result.conflicts), "error");
        return;
    }

    titleInput->clear();
    descriptionInput->clear();

    showStatus(result.message, "success");
    refreshAll();

    treeRoomCombo->setCurrentText(bookingRoomCombo->currentText());
    refreshTreeText();
}

void MainWindow::checkAvailability()
{
    OperationResult result = scheduler.checkAvailability(
        checkRoomCombo->currentText(),
        qtimeToMinutes(checkStartTimeEdit->time()),
        qtimeToMinutes(checkEndTimeEdit->time())
    );

    if (!result.ok) {
        showStatus(result.message, "error");
        return;
    }

    if (result.free) {
        showStatus(result.message, "success");
    } else {
        showStatus(result.message + conflictsToText(result.conflicts), "error");
    }
}

void MainWindow::updateSelectedBooking()
{
    int row = bookingTable->currentRow();

    if (row < 0) {
        showStatus("Select a booking from the table first.", "error");
        return;
    }

    QString roomName = bookingTable->item(row, 0)->data(Qt::UserRole).toString();
    int bookingIndex = bookingTable->item(row, 0)->data(Qt::UserRole + 1).toInt();

    QVector<Booking> bookings = scheduler.getBookings(roomName);

    if (bookingIndex < 0 || bookingIndex >= bookings.size()) {
        showStatus("Selected booking not found.", "error");
        return;
    }

    Booking oldBooking = bookings[bookingIndex];

    QDialog dialog(this);
    dialog.setWindowTitle("Update Booking");

    QFormLayout* layout = new QFormLayout(&dialog);

    QTimeEdit* startEdit = new QTimeEdit(minutesToQTime(oldBooking.start));
    QTimeEdit* endEdit = new QTimeEdit(minutesToQTime(oldBooking.end));
    QLineEdit* titleEdit = new QLineEdit(oldBooking.title);
    QTextEdit* descEdit = new QTextEdit(oldBooking.description);

    startEdit->setDisplayFormat("HH:mm");
    endEdit->setDisplayFormat("HH:mm");
    descEdit->setFixedHeight(70);

    layout->addRow("Start:", startEdit);
    layout->addRow("End:", endEdit);
    layout->addRow("Title:", titleEdit);
    layout->addRow("Description:", descEdit);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    OperationResult result = scheduler.updateBooking(
        roomName,
        bookingIndex,
        qtimeToMinutes(startEdit->time()),
        qtimeToMinutes(endEdit->time()),
        titleEdit->text(),
        descEdit->toPlainText()
    );

    if (!result.ok) {
        showStatus(result.message + conflictsToText(result.conflicts), "error");
        return;
    }

    showStatus(result.message, "success");
    refreshAll();

    treeRoomCombo->setCurrentText(roomName);
    refreshTreeText();
}

void MainWindow::cancelSelectedBooking()
{
    int row = bookingTable->currentRow();

    if (row < 0) {
        showStatus("Select a booking from the table first.", "error");
        return;
    }

    QString roomName = bookingTable->item(row, 0)->data(Qt::UserRole).toString();
    int bookingIndex = bookingTable->item(row, 0)->data(Qt::UserRole + 1).toInt();

    OperationResult result = scheduler.cancelBooking(roomName, bookingIndex);

    if (!result.ok) {
        showStatus(result.message, "error");
        return;
    }

    showStatus(result.message, "success");
    refreshAll();

    treeRoomCombo->setCurrentText(roomName);
    refreshTreeText();
}

int MainWindow::qtimeToMinutes(const QTime& time) const
{
    return time.hour() * 60 + time.minute();
}

QTime MainWindow::minutesToQTime(int minutes) const
{
    return QTime(minutes / 60, minutes % 60);
}

QString MainWindow::formatTime(int minutes) const
{
    int h = minutes / 60;
    int m = minutes % 60;

    QString suffix = h < 12 ? "AM" : "PM";
    int displayHour = h % 12;

    if (displayHour == 0) {
        displayHour = 12;
    }

    return QString("%1:%2 %3")
        .arg(displayHour)
        .arg(m, 2, 10, QChar('0'))
        .arg(suffix);
}

void MainWindow::showStatus(const QString& message, const QString& type)
{
    statusLabel->setText(message);

    if (type == "success") {
        statusLabel->setObjectName("statusSuccess");
    } else if (type == "error") {
        statusLabel->setObjectName("statusError");
    } else {
        statusLabel->setObjectName("statusInfo");
    }

    statusLabel->style()->unpolish(statusLabel);
    statusLabel->style()->polish(statusLabel);
}

QString MainWindow::conflictsToText(const QVector<Booking>& conflicts) const
{
    if (conflicts.isEmpty()) {
        return "";
    }

    QString text = "\n\nConflicts with:";

    for (const Booking& booking : conflicts) {
        text += "\n- " + formatTime(booking.start) + " - " + formatTime(booking.end)
              + " | " + booking.title;
    }

    return text;
}

QString MainWindow::buildTreeText(const QVector<Booking>& bookings)
{
    QVector<Booking> sorted = bookings;

    std::sort(sorted.begin(), sorted.end(), [](const Booking& a, const Booking& b) {
        return a.start < b.start;
    });

    QString text;
    text += "Interval Tree Visual Representation\n";
    text += "-----------------------------------\n";
    text += "Each node shows: [start - end] title | max end in subtree\n\n";

    text += renderTreeNode(sorted, 0, sorted.size() - 1, 0);

    return text;
}

QString MainWindow::renderTreeNode(const QVector<Booking>& bookings, int left, int right, int depth)
{
    if (left > right) {
        return "";
    }

    int mid = (left + right) / 2;
    const Booking& booking = bookings[mid];

    QString indent(depth * 4, ' ');

    QString nodeType = depth == 0 ? "Root" : "Node";
    int maxEnd = maxEndInRange(bookings, left, right);

    QString text;
    text += indent + nodeType + ": ";
    text += "[" + formatTime(booking.start) + " - " + formatTime(booking.end) + "] ";
    text += booking.title + " | max = " + formatTime(maxEnd) + "\n";

    text += renderTreeNode(bookings, left, mid - 1, depth + 1);
    text += renderTreeNode(bookings, mid + 1, right, depth + 1);

    return text;
}

int MainWindow::maxEndInRange(const QVector<Booking>& bookings, int left, int right)
{
    int maxEnd = 0;

    for (int i = left; i <= right; i++) {
        maxEnd = qMax(maxEnd, bookings[i].end);
    }

    return maxEnd;
}