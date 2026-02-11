#include "worktimetracker.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QStyle>
#include <QToolTip>
#include <QCursor>

WorkTimeTracker::WorkTimeTracker(QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_updateTimer(nullptr)
    , m_warningShown(false)
{
    // Load existing session or create new one
    loadOrCreateSession();
    
    // Setup system tray
    setupTrayIcon();

    // Setup update timer (update every minute)
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &WorkTimeTracker::updateElapsedTime);
    m_updateTimer->start(60000); // 60 seconds

    // Initial update
    updateElapsedTime();
}

WorkTimeTracker::~WorkTimeTracker()
{
    // Save the current session
    saveSession();
    
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

void WorkTimeTracker::setupTrayIcon()
{
    // Create tray icon
    m_trayIcon = new QSystemTrayIcon(this);
    
    // Use a simple icon (you can replace this with a custom icon file)
    QIcon icon = QIcon::fromTheme("appointment-soon", 
                                   QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
    m_trayIcon->setIcon(icon);

    // Create context menu
    m_trayMenu = new QMenu();
    
    QAction *showTimeAction = new QAction("Show Work Time", this);
    connect(showTimeAction, &QAction::triggered, this, &WorkTimeTracker::showTimeInfo);
    
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &WorkTimeTracker::quitApplication);
    
    m_trayMenu->addAction(showTimeAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(quitAction);
    
    m_trayIcon->setContextMenu(m_trayMenu);
    
    // Connect activation signal
    connect(m_trayIcon, &QSystemTrayIcon::activated, 
            this, &WorkTimeTracker::onTrayIconActivated);
    
    // Show the tray icon
    m_trayIcon->show();
    m_trayIcon->setToolTip("Work Time Tracker - Started");
}

void WorkTimeTracker::logStartTime()
{
    // Get the data directory
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString logFilePath = dataDir + "/worktime.log";
    QFile logFile(logFilePath);
    
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "Work session started: " 
            << m_startTime.toString("yyyy-MM-dd hh:mm:ss") 
            << "\n";
        logFile.close();
    }
}

QString WorkTimeTracker::formatElapsedTime(qint64 seconds) const
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
}

void WorkTimeTracker::updateElapsedTime()
{
    qint64 elapsedSeconds = m_startTime.secsTo(QDateTime::currentDateTime());
    QString elapsedStr = formatElapsedTime(elapsedSeconds);
    
    // Calculate estimated leaving time
    QDateTime leavingTime = m_startTime.addSecs(WORK_TIME_LIMIT);
    QString leavingTimeStr = leavingTime.toString("HH:mm");

    // Update tooltip
    m_trayIcon->setToolTip(QString("Work Time: %1 / 8:30:00\nEstimated Leaving Time: %2")
                               .arg(elapsedStr)
                               .arg(leavingTimeStr));
    
    // Check if work time limit is reached
    checkWorkTimeLimit();
}

void WorkTimeTracker::checkWorkTimeLimit()
{
    qint64 elapsedSeconds = m_startTime.secsTo(QDateTime::currentDateTime());
    
    if (elapsedSeconds >= WORK_TIME_LIMIT && !m_warningShown) {
        m_warningShown = true;
        
        QString message = QString("Your daily work time of 8:30:00 has been reached!\n\n"
                                 "Total time worked: %1")
                         .arg(formatElapsedTime(elapsedSeconds));
        
        // Show system tray message
        m_trayIcon->showMessage("Work Time Alert", 
                               message,
                               QSystemTrayIcon::Warning,
                               10000); // Show for 10 seconds
        
        // Also log this event
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString logFilePath = dataDir + "/worktime.log";
        QFile logFile(logFilePath);
        
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << "Work time limit reached: " 
                << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") 
                << " (Duration: " << formatElapsedTime(elapsedSeconds) << ")\n";
            logFile.close();
        }
    }
}

void WorkTimeTracker::showTimeInfo()
{
    qint64 elapsedSeconds = m_startTime.secsTo(QDateTime::currentDateTime());
    QString elapsedStr = formatElapsedTime(elapsedSeconds);
    
    // Calculate estimated leaving time
    QDateTime leavingTime = m_startTime.addSecs(WORK_TIME_LIMIT);
    QString leavingTimeStr = leavingTime.toString("HH:mm");

    QString info = QString("Work Session Information\n\n"
                          "Started: %1\n"
                          "Current Time: %2\n"
                          "Elapsed: %3\n"
                          "Target: 8:30:00\n"
                          "Estimated Leaving Time: %4")
                   .arg(m_startTime.toString("yyyy-MM-dd hh:mm:ss"))
                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                   .arg(elapsedStr)
                   .arg(leavingTimeStr);
    
    QMessageBox::information(nullptr, "Work Time Tracker", info);
}

void WorkTimeTracker::quitApplication()
{
    // Log the end time
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString logFilePath = dataDir + "/worktime.log";
    QFile logFile(logFilePath);
    
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        QDateTime endTime = QDateTime::currentDateTime();
        qint64 elapsedSeconds = m_startTime.secsTo(endTime);
        
        out << "Work session ended: " 
            << endTime.toString("yyyy-MM-dd hh:mm:ss") 
            << " (Duration: " << formatElapsedTime(elapsedSeconds) << ")\n";
        out << "----------------------------------------\n";
        logFile.close();
    }
    
    QApplication::quit();
}

void WorkTimeTracker::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showTimeInfo();
    } else if (reason == QSystemTrayIcon::Trigger) {
        m_trayIcon->showMessage("Work Time Info", m_trayIcon->toolTip(), QSystemTrayIcon::Information, 5000);
    }
}

QString WorkTimeTracker::getSessionFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return dataDir + "/session.dat";
}

QDateTime WorkTimeTracker::getEarliestStartTimeToday() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString logFilePath = dataDir + "/worktime.log";
    QFile logFile(logFilePath);
    
    QDateTime earliestTime;
    QDate today = QDate::currentDate();
    
    if (!logFile.exists()) {
        return earliestTime; // Return invalid QDateTime if no log file
    }
    
    if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&logFile);
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            
            // Look for "Work session started:" entries
            if (line.startsWith("Work session started: ")) {
                // Extract the timestamp
                QString timestampStr = line.mid(22); // Skip "Work session started: "
                QDateTime timestamp = QDateTime::fromString(timestampStr, "yyyy-MM-dd hh:mm:ss");
                
                // Check if it's from today and is the earliest so far
                if (timestamp.isValid() && timestamp.date() == today) {
                    if (!earliestTime.isValid() || timestamp < earliestTime) {
                        earliestTime = timestamp;
                    }
                }
            }
        }
        
        logFile.close();
    }
    
    return earliestTime;
}

void WorkTimeTracker::loadOrCreateSession()
{
    bool sessionLoaded = false;
    QDateTime today = QDateTime::currentDateTime();
    
    // First, check if there's an existing start time in today's log file
    QDateTime earliestStartTime = getEarliestStartTimeToday();
    
    if (earliestStartTime.isValid()) {
        // Use the earliest start time from the log file
        m_startTime = earliestStartTime;
        sessionLoaded = true;
        
        qint64 elapsedSeconds = m_startTime.secsTo(QDateTime::currentDateTime());
        
        // Log session resumption
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        
        QString logFilePath = dataDir + "/worktime.log";
        QFile logFile(logFilePath);
        
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << "Session resumed: " 
                << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                << " (Started at: " << m_startTime.toString("yyyy-MM-dd hh:mm:ss")
                << ", Elapsed: " << formatElapsedTime(elapsedSeconds) << ")\n";
            logFile.close();
        }
        
        // Check if warning was already shown (if elapsed time >= limit)
        if (elapsedSeconds >= WORK_TIME_LIMIT) {
            m_warningShown = true;
        }
        
        // Update session file to keep it in sync
        saveSession();
    } else {
        // No existing session found for today, create a new one
        m_startTime = QDateTime::currentDateTime();
        logStartTime();
        saveSession();
    }
}

void WorkTimeTracker::saveSession()
{
    QString sessionFilePath = getSessionFilePath();
    
    // Ensure directory exists
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QFile sessionFile(sessionFilePath);
    if (sessionFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&sessionFile);
        out << m_startTime.toString("yyyy-MM-dd hh:mm:ss");
        sessionFile.close();
    }
}

