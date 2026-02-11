#include <QApplication>
#include <QMessageBox>
#include "worktimetracker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("WorkTimeTracker");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("WorkTimer");
    
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Work Time Tracker",
                            "System tray is not available on this system.");
        return 1;
    }
    
    // Prevent application from quitting when last window closes
    QApplication::setQuitOnLastWindowClosed(false);
    
    // Create the work time tracker
    WorkTimeTracker tracker;
    
    return app.exec();
}
