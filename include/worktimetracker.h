#ifndef WORKTIMETRACKER_H
#define WORKTIMETRACKER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDateTime>
#include <QMenu>
#include <QAction>

class WorkTimeTracker : public QObject
{
    Q_OBJECT

public:
    explicit WorkTimeTracker(QObject *parent = nullptr);
    ~WorkTimeTracker();

private slots:
    void updateElapsedTime();
    void showTimeInfo();
    void quitApplication();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupTrayIcon();
    void logStartTime();
    QString formatElapsedTime(qint64 seconds) const;
    void checkWorkTimeLimit();
    
    // Session persistence methods
    void loadOrCreateSession();
    void saveSession();
    QString getSessionFilePath() const;
    QDateTime getEarliestStartTimeToday() const;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QTimer *m_updateTimer;
    QDateTime m_startTime;
    
    // 8 hours 30 minutes in seconds
    const qint64 WORK_TIME_LIMIT = 8 * 3600 + 30 * 60; // 30600 seconds
    bool m_warningShown;
};

#endif // WORKTIMETRACKER_H
