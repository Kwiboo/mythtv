// -*- Mode: c++ -*-

#ifndef MYTH_SIGNALING_TIMER_H
#define MYTH_SIGNALING_TIMER_H

#include <cstdint>

#include <QWaitCondition>
#include <QMutex>

#include "mythbaseexp.h"
#include "mthread.h"

/** \class MythSignalingTimer
 *  This class is essentially a workaround for a Qt 4.5.2 bug where it
 *  will get stuck in the Qt event loop for up to 999 nanoseconds per
 *  timer firing. This lost millisecond is not a huge issue for infrequent
 *  timers, but causes 7% lost CPU in the MythUI animate() handling.
 */
class MBASE_PUBLIC MythSignalingTimer : public QObject, private MThread
{
    Q_OBJECT

  public:
    template <class OBJ, typename FUNC>
    MythSignalingTimer(OBJ *parent, FUNC func) :
        QObject(parent), MThread("SignalingTimer")
    {
        connect(this, &MythSignalingTimer::timeout, parent, func,
                Qt::QueuedConnection);
    }
    ~MythSignalingTimer() override;

    virtual void stop(void);
    virtual void start(int msec);

    virtual bool blockSignals(bool block)
        { return QObject::blockSignals(block); }
    bool isActive(void) const
        { return m_dorun; }

  signals:
    void timeout(void);

  private:
    void run(void) override; // MThread

    QMutex            m_startStopLock;
    QWaitCondition    m_timerWait;
    volatile bool     m_dorun    {false};
    volatile bool     m_running  {false};
    volatile uint64_t m_millisec {0};
};

#endif // MYTH_SIGNALING_TIMER_H
