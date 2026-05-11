let rooms = [];

let alertTimer = null;

function toMinutes(time) {
  const [h, m] = time.split(":").map(Number);
  return h * 60 + m;
}

function formatTime(time) {
  const [h, m] = time.split(":").map(Number);
  const hour = h % 12 || 12;
  const suffix = h < 12 ? "AM" : "PM";
  return `${hour}:${String(m).padStart(2, "0")} ${suffix}`;
}

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function escapeJs(value) {
  return String(value)
    .replaceAll("\\", "\\\\")
    .replaceAll("'", "\\'");
}

function isValidTime(time) {
  return /^([01]\d|2[0-3]):([0-5]\d)$/.test(time);
}

function hasConflict(bookings, start, end) {
  const newStart = toMinutes(start);
  const newEnd = toMinutes(end);

  return bookings.filter(booking => {
    const existingStart = toMinutes(booking.start);
    const existingEnd = toMinutes(booking.end);

    return newStart < existingEnd && newEnd > existingStart;
  });
}

function addRoom() {
  const input = document.getElementById("roomName");
  const name = input.value.trim();

  if (!name) {
    showAlert("Room name is required.", "error", true);
    return;
  }

  const exists = rooms.some(room => room.name.toLowerCase() === name.toLowerCase());

  if (exists) {
    showAlert(`Room "${escapeHtml(name)}" already exists.`, "error", true);
    return;
  }

  rooms.push({
    name,
    bookings: []
  });

  input.value = "";

  showAlert(`Room "${escapeHtml(name)}" added successfully.`, "success");
  render();
}

function bookMeeting() {
  const roomName = document.getElementById("bookingRoom").value;
  const start = document.getElementById("startTime").value;
  const end = document.getElementById("endTime").value;
  const label = document.getElementById("meetingLabel").value.trim();
  const description = document.getElementById("meetingDescription").value.trim();

  if (!roomName) {
    showAlert("Add a room first.", "error", true);
    return;
  }

  if (!start || !end || !label) {
    showAlert("Fill room, start time, end time, and meeting title.", "error", true);
    return;
  }

  if (toMinutes(start) >= toMinutes(end)) {
    showAlert("End time must be after start time.", "error", true);
    return;
  }

  const room = rooms.find(room => room.name === roomName);

  if (!room) {
    showAlert("Selected room was not found.", "error", true);
    return;
  }

  const conflicts = hasConflict(room.bookings, start, end);

  if (conflicts.length > 0) {
    const clashText = conflicts
      .map(c => `${formatTime(c.start)} - ${formatTime(c.end)} | ${escapeHtml(c.label)}`)
      .join("<br>");

    showAlert(
      `
        <span class="alert-title">Conflict detected</span>
        Cannot book "${escapeHtml(label)}" in ${escapeHtml(roomName)} from ${formatTime(start)} to ${formatTime(end)}.
        <br><br>
        <strong>Clashes with:</strong><br>
        ${clashText}
      `,
      "error",
      true
    );

    return;
  }

  room.bookings.push({
    start,
    end,
    label,
    description
  });

  room.bookings.sort((a, b) => toMinutes(a.start) - toMinutes(b.start));

  document.getElementById("meetingLabel").value = "";
  document.getElementById("meetingDescription").value = "";

  showAlert(
    `Booked "${escapeHtml(label)}" in ${escapeHtml(roomName)} from ${formatTime(start)} to ${formatTime(end)}.`,
    "success"
  );

  render();

  document.getElementById("treeRoom").value = roomName;
  renderTreeView();
}

function cancelBooking(roomName, index) {
  const room = rooms.find(room => room.name === roomName);

  if (!room || index < 0 || index >= room.bookings.length) {
    showAlert("Booking not found.", "error", true);
    return;
  }

  const removed = room.bookings.splice(index, 1)[0];

  showAlert(`Cancelled "${escapeHtml(removed.label)}" in ${escapeHtml(roomName)}.`, "info");
  render();

  document.getElementById("treeRoom").value = roomName;
  renderTreeView();
}

function updateBooking(roomName, index) {
  const room = rooms.find(room => room.name === roomName);

  if (!room || index < 0 || index >= room.bookings.length) {
    showAlert("Booking not found.", "error", true);
    return;
  }

  const oldBooking = room.bookings[index];

  const newStart = prompt("New start time in 24-hour format, example 13:30:", oldBooking.start);
  const newEnd = prompt("New end time in 24-hour format, example 14:30:", oldBooking.end);
  const newLabel = prompt("New meeting title:", oldBooking.label);
  const newDescription = prompt("New meeting description:", oldBooking.description || "");

  if (!newStart || !newEnd || !newLabel) {
    showAlert("Update cancelled.", "info");
    return;
  }

  if (!isValidTime(newStart) || !isValidTime(newEnd)) {
    showAlert("Use valid 24-hour time format, like 09:30 or 14:00.", "error", true);
    return;
  }

  if (toMinutes(newStart) >= toMinutes(newEnd)) {
    showAlert("End time must be after start time.", "error", true);
    return;
  }

  const otherBookings = room.bookings.filter((_, i) => i !== index);
  const conflicts = hasConflict(otherBookings, newStart, newEnd);

  if (conflicts.length > 0) {
    const clashText = conflicts
      .map(c => `${formatTime(c.start)} - ${formatTime(c.end)} | ${escapeHtml(c.label)}`)
      .join("<br>");

    showAlert(
      `
        <span class="alert-title">Conflict detected</span>
        Cannot update this meeting.
        <br><br>
        <strong>Clashes with:</strong><br>
        ${clashText}
      `,
      "error",
      true
    );

    return;
  }

  room.bookings[index] = {
    start: newStart,
    end: newEnd,
    label: newLabel.trim(),
    description: newDescription ? newDescription.trim() : ""
  };

  room.bookings.sort((a, b) => toMinutes(a.start) - toMinutes(b.start));

  showAlert(`Updated booking in ${escapeHtml(roomName)}.`, "success");
  render();

  document.getElementById("treeRoom").value = roomName;
  renderTreeView();
}

function checkAvailability() {
  const roomName = document.getElementById("checkRoom").value;
  const start = document.getElementById("checkStart").value;
  const end = document.getElementById("checkEnd").value;

  if (!roomName) {
    showAlert("Add a room first.", "error", true);
    return;
  }

  if (!start || !end) {
    showAlert("Select start and end time to check availability.", "error", true);
    return;
  }

  if (toMinutes(start) >= toMinutes(end)) {
    showAlert("End time must be after start time.", "error", true);
    return;
  }

  const room = rooms.find(room => room.name === roomName);

  if (!room) {
    showAlert("Selected room was not found.", "error", true);
    return;
  }

  const conflicts = hasConflict(room.bookings, start, end);

  if (conflicts.length === 0) {
    showAlert(
      `${escapeHtml(roomName)} is free from ${formatTime(start)} to ${formatTime(end)}.`,
      "success"
    );
    return;
  }

  const clashText = conflicts
    .map(c => `${formatTime(c.start)} - ${formatTime(c.end)} | ${escapeHtml(c.label)}`)
    .join("<br>");

  showAlert(
    `
      <span class="alert-title">Room is busy</span>
      ${escapeHtml(roomName)} is not available from ${formatTime(start)} to ${formatTime(end)}.
      <br><br>
      <strong>Conflicting bookings:</strong><br>
      ${clashText}
    `,
    "error",
    true
  );
}

function showAlert(message, type = "info", persistent = false) {
  const alertBox = document.getElementById("alertBox");

  if (alertTimer) {
    clearTimeout(alertTimer);
    alertTimer = null;
  }

  const shouldPersist = persistent || type === "error";

  alertBox.innerHTML = `
    <div class="alert-content">${message}</div>
    <button class="alert-close" onclick="closeAlert()">×</button>
  `;

  alertBox.className = `alert ${type}`;

  if (!shouldPersist) {
    alertTimer = setTimeout(() => {
      closeAlert();
    }, 5000);
  }
}

function closeAlert() {
  const alertBox = document.getElementById("alertBox");
  alertBox.className = "alert hidden";
  alertBox.innerHTML = "";
}

function showEventTooltip(event, roomName, booking) {
  const tooltip = document.getElementById("eventTooltip");

  tooltip.innerHTML = `
    <span class="tooltip-badge">${escapeHtml(roomName)}</span>
    <h4>${escapeHtml(booking.label)}</h4>
    <div class="tooltip-meta">
      ${formatTime(booking.start)} - ${formatTime(booking.end)}
    </div>
    <div class="tooltip-desc">
      ${escapeHtml(booking.description || "No description added.")}
    </div>
  `;

  tooltip.className = "event-tooltip";
  moveEventTooltip(event);
}

function moveEventTooltip(event) {
  const tooltip = document.getElementById("eventTooltip");

  const offset = 16;
  let left = event.clientX + offset;
  let top = event.clientY + offset;

  const rect = tooltip.getBoundingClientRect();

  if (left + rect.width > window.innerWidth) {
    left = event.clientX - rect.width - offset;
  }

  if (top + rect.height > window.innerHeight) {
    top = event.clientY - rect.height - offset;
  }

  tooltip.style.left = `${left}px`;
  tooltip.style.top = `${top}px`;
}

function hideEventTooltip() {
  const tooltip = document.getElementById("eventTooltip");
  tooltip.className = "event-tooltip hidden";
}

function renderRooms() {
  const roomList = document.getElementById("roomList");
  roomList.innerHTML = "";

  if (rooms.length === 0) {
    roomList.innerHTML = `<div class="empty-state">No rooms added yet.</div>`;
    return;
  }

  rooms.forEach(room => {
    const div = document.createElement("div");
    div.className = "room-item";

    div.innerHTML = `
      <strong>${escapeHtml(room.name)}</strong>
      <small>${room.bookings.length} ${room.bookings.length === 1 ? "booking" : "bookings"}</small>
    `;

    roomList.appendChild(div);
  });
}

function renderDropdowns() {
  const bookingRoom = document.getElementById("bookingRoom");
  const checkRoom = document.getElementById("checkRoom");
  const treeRoom = document.getElementById("treeRoom");

  const selectedBookingRoom = bookingRoom.value;
  const selectedCheckRoom = checkRoom.value;
  const selectedTreeRoom = treeRoom.value;

  bookingRoom.innerHTML = "";
  checkRoom.innerHTML = "";
  treeRoom.innerHTML = "";

  if (rooms.length === 0) {
    bookingRoom.innerHTML = `<option value="">Add a room first</option>`;
    checkRoom.innerHTML = `<option value="">Add a room first</option>`;
    treeRoom.innerHTML = `<option value="">Add a room first</option>`;
    return;
  }

  rooms.forEach(room => {
    const bookingOption = document.createElement("option");
    bookingOption.value = room.name;
    bookingOption.textContent = room.name;
    bookingRoom.appendChild(bookingOption);

    const checkOption = document.createElement("option");
    checkOption.value = room.name;
    checkOption.textContent = room.name;
    checkRoom.appendChild(checkOption);

    const treeOption = document.createElement("option");
    treeOption.value = room.name;
    treeOption.textContent = room.name;
    treeRoom.appendChild(treeOption);
  });

  bookingRoom.value = rooms.some(r => r.name === selectedBookingRoom)
    ? selectedBookingRoom
    : rooms[0].name;

  checkRoom.value = rooms.some(r => r.name === selectedCheckRoom)
    ? selectedCheckRoom
    : rooms[0].name;

  treeRoom.value = rooms.some(r => r.name === selectedTreeRoom)
    ? selectedTreeRoom
    : rooms[0].name;
}

function renderStats() {
  const totalBookings = rooms.reduce((sum, room) => sum + room.bookings.length, 0);

  document.getElementById("totalRooms").textContent = rooms.length;
  document.getElementById("totalBookings").textContent = totalBookings;
}

function renderTimelineScale() {
  const scale = document.getElementById("timelineScale");

  let labels = "";

  for (let hour = 8; hour <= 20; hour++) {
    const displayHour = hour % 12 === 0 ? 12 : hour % 12;
    const suffix = hour < 12 ? "AM" : "PM";
    labels += `<div>${displayHour} ${suffix}</div>`;
  }

  scale.innerHTML = `
    <div></div>
    <div class="timeline-scale-labels">${labels}</div>
  `;
}

function renderTimeline() {
  const timeline = document.getElementById("timeline");
  timeline.innerHTML = "";

  if (rooms.length === 0) {
    timeline.innerHTML = `<div class="empty-state">Add a room to start using the timeline.</div>`;
    return;
  }

  const startDay = 8 * 60;
  const endDay = 20 * 60;
  const totalMinutes = endDay - startDay;

  rooms.forEach(room => {
    const row = document.createElement("div");
    row.className = "timeline-row";

    const label = document.createElement("div");
    label.className = "room-label";
    label.textContent = room.name;

    const track = document.createElement("div");
    track.className = "timeline-track";

    room.bookings.forEach(booking => {
      const start = toMinutes(booking.start);
      const end = toMinutes(booking.end);

      const visibleStart = Math.max(start, startDay);
      const visibleEnd = Math.min(end, endDay);

      if (visibleEnd <= startDay || visibleStart >= endDay) {
        return;
      }

      const left = ((visibleStart - startDay) / totalMinutes) * 100;
      const width = ((visibleEnd - visibleStart) / totalMinutes) * 100;

      const block = document.createElement("div");
      block.className = "booking-block";
      block.style.left = `${left}%`;
      block.style.width = `${Math.max(width, 7)}%`;

      block.innerHTML = `
        <span class="booking-title">${escapeHtml(booking.label)}</span>
        <span class="booking-time">${formatTime(booking.start)} - ${formatTime(booking.end)}</span>
        <span class="booking-desc-preview">${escapeHtml(booking.description || "No description")}</span>
      `;

      block.addEventListener("mouseenter", event => {
        showEventTooltip(event, room.name, booking);
      });

      block.addEventListener("mousemove", event => {
        moveEventTooltip(event);
      });

      block.addEventListener("mouseleave", () => {
        hideEventTooltip();
      });

      block.addEventListener("click", event => {
        event.stopPropagation();

        showAlert(
          `
            <span class="alert-title">${escapeHtml(booking.label)}</span>
            <strong>Room:</strong> ${escapeHtml(room.name)}<br>
            <strong>Time:</strong> ${formatTime(booking.start)} - ${formatTime(booking.end)}<br>
            <strong>Description:</strong> ${escapeHtml(booking.description || "No description added.")}
          `,
          "info",
          true
        );
      });

      track.appendChild(block);
    });

    row.appendChild(label);
    row.appendChild(track);
    timeline.appendChild(row);
  });
}

function renderBookingList() {
  const bookingList = document.getElementById("bookingList");
  bookingList.innerHTML = "";

  let hasAny = false;

  rooms.forEach(room => {
    room.bookings.forEach((booking, index) => {
      hasAny = true;

      const item = document.createElement("div");
      item.className = "booking-item";

      item.innerHTML = `
        <strong>${escapeHtml(booking.label)}</strong>
        <div class="booking-meta">
          ${escapeHtml(room.name)} | ${formatTime(booking.start)} - ${formatTime(booking.end)}
        </div>
        <div class="booking-meta">
          ${escapeHtml(booking.description || "No description")}
        </div>
        <div class="booking-actions">
          <button class="warning" onclick="updateBooking('${escapeJs(room.name)}', ${index})">Update</button>
          <button class="danger" onclick="cancelBooking('${escapeJs(room.name)}', ${index})">Cancel</button>
        </div>
      `;

      bookingList.appendChild(item);
    });
  });

  if (!hasAny) {
    bookingList.innerHTML = `<div class="empty-state">No bookings yet.</div>`;
  }
}

function renderTreeView() {
  const treeView = document.getElementById("treeView");
  const treeRoom = document.getElementById("treeRoom");

  treeView.innerHTML = "";

  if (!rooms.length || !treeRoom.value) {
    treeView.innerHTML = `<div class="empty-state">No intervals to show yet.</div>`;
    return;
  }

  const room = rooms.find(room => room.name === treeRoom.value);

  if (!room || room.bookings.length === 0) {
    treeView.innerHTML = `<div class="empty-state">No intervals to show for this room.</div>`;
    return;
  }

  const sorted = [...room.bookings].sort((a, b) => toMinutes(a.start) - toMinutes(b.start));
  const root = buildBalancedIntervalTree(sorted);

  treeView.innerHTML = `
    <div class="tree-wrap">
      <ul class="tree">
        ${renderTreeMarkup(root, true)}
      </ul>
    </div>
  `;
}

function buildBalancedIntervalTree(bookings) {
  if (bookings.length === 0) return null;

  const mid = Math.floor(bookings.length / 2);

  return {
    booking: bookings[mid],
    left: buildBalancedIntervalTree(bookings.slice(0, mid)),
    right: buildBalancedIntervalTree(bookings.slice(mid + 1))
  };
}

function renderTreeMarkup(node, isRoot = false) {
  if (!node) return "";

  const maxEnd = findMaxEnd(node);
  const hasChildren = node.left || node.right;

  return `
    <li>
      <div class="tree-node-card ${isRoot ? "root-node" : ""}">
        <span class="node-chip">${isRoot ? "Root" : "Node"}</span>
        <div class="node-range">[${formatTime(node.booking.start)} - ${formatTime(node.booking.end)}]</div>
        <div class="node-label">${escapeHtml(node.booking.label)}</div>
        <div class="node-max">max = ${formatTime(maxEnd)}</div>
      </div>

      ${
        hasChildren
          ? `
            <ul>
              ${node.left ? renderTreeMarkup(node.left, false) : ""}
              ${node.right ? renderTreeMarkup(node.right, false) : ""}
            </ul>
          `
          : ""
      }
    </li>
  `;
}

function findMaxEnd(node) {
  if (!node) return "00:00";

  const values = [node.booking.end];

  if (node.left) values.push(findMaxEnd(node.left));
  if (node.right) values.push(findMaxEnd(node.right));

  return values.sort((a, b) => toMinutes(b) - toMinutes(a))[0];
}

function render() {
  renderRooms();
  renderDropdowns();
  renderStats();
  renderTimelineScale();
  renderTimeline();
  renderBookingList();
  renderTreeView();
}

document.addEventListener("change", function (event) {
  if (event.target.id === "bookingRoom") {
    const selectedRoom = event.target.value;
    const treeRoom = document.getElementById("treeRoom");

    if (treeRoom) {
      treeRoom.value = selectedRoom;
    }

    renderTreeView();
  }

  if (event.target.id === "treeRoom") {
    renderTreeView();
  }
});

document.addEventListener("click", function () {
  hideEventTooltip();
});

render();