# Work Time Tracker

A Qt-based application that tracks your daily work time and notifies you when your daily limit (8 hours 30 minutes) is reached.

## Features

- **Automatic Time Logging**: Logs the start time when the application launches
- **System Tray Integration**: Runs in the system tray, always accessible
- **Real-time Tracking**: Updates your work time every minute
- **Notifications**: Alerts you when you've reached your 8:30 daily work limit
- **Session Logging**: Maintains a log file of all work sessions

## Requirements

- Qt5 (5.15 or later) or Qt6
- CMake 3.16 or later
- C++17 compatible compiler

## Project Structure

```
working_time/
├── CMakeLists.txt          # Build configuration
├── README.md               # Documentation
├── include/                # Header files
│   └── worktimetracker.h
├── src/                    # Source files
│   ├── main.cpp
│   └── worktimetracker.cpp
└── build/                  # Build directory
    └── WorkTimeTracker     # Compiled executable
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Installation

```bash
sudo make install
```

Or run directly from the build directory:

```bash
./WorkTimeTracker
```

## Usage

1. **Starting the Application**: Simply launch `WorkTimeTracker`. It will appear in your system tray.

2. **System Tray Icon**: 
   - **Hover**: See current work time vs target (8:30:00)
   - **Double-click**: Show detailed time information
   - **Right-click**: Access context menu

3. **Context Menu Options**:
   - **Show Work Time**: Display detailed session information
   - **Quit**: Exit the application (logs end time)

4. **Notifications**: When you reach 8:30 hours of work, a notification will appear in your system tray.

5. **Log Files**: Session logs are stored in:
   - Linux: `~/.local/share/WorkTimer/WorkTimeTracker/worktime.log`
   - The log includes start time, end time, and duration for each session

## Log Format

```
Work session started: 2026-02-11 07:28:24
Work time limit reached: 2026-02-11 15:58:24 (Duration: 08:30:00)
Work session ended: 2026-02-11 16:30:45 (Duration: 09:02:21)
----------------------------------------
```

## Customization

To change the daily work time limit, edit `worktimetracker.h`:

```cpp
const qint64 WORK_TIME_LIMIT = 8 * 3600 + 30 * 60; // 8:30 in seconds
```

For example, for 7 hours 45 minutes:
```cpp
const qint64 WORK_TIME_LIMIT = 7 * 3600 + 45 * 60; // 7:45 in seconds
```

## Troubleshooting

**System tray icon not appearing:**
- Ensure your desktop environment supports system tray icons
- Some modern desktops require extensions (e.g., GNOME Shell requires TopIcons Plus or AppIndicator)

**Application won't start:**
- Check that Qt libraries are installed: `ldd ./WorkTimeTracker`
- Verify system tray availability on your system

## License

This is a personal work time tracking tool.
