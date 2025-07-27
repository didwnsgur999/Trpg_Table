#ifndef RESPONSETIMER_H
#define RESPONSETIMER_H

#include <QElapsedTimer>
#include <QMap>
#include <QString>
#include <QDebug>

class ResponseTimer
{
public:
    static ResponseTimer& getInstance() {
        static ResponseTimer instance;
        return instance;
    }

    void start(const QString& label) {
        QElapsedTimer timer;
        timer.start();
        m_timers[label] = timer;
    }

    void stop(const QString& label) {
        if (m_timers.contains(label)) {
            qint64 elapsed = m_timers[label].elapsed();
            qDebug() << "[응답시간]" << label << ":" << elapsed << "ms";
            m_timers.remove(label);
        } else {
            qDebug() << "[경고] 타이머가 존재하지 않음:" << label;
        }
    }

private:
    QMap<QString, QElapsedTimer> m_timers;

    // 생성자/소멸자 private
    ResponseTimer() {}
    ~ResponseTimer() {}
    Q_DISABLE_COPY(ResponseTimer)
};

#endif // RESPONSETIMER_H
