#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>
#include <QTimer>
#include "ConfigLoader.h"
#include "FrameHandler.h"

class StateMachine : public QObject
{
    Q_OBJECT
public:
    explicit StateMachine(QObject *parent = nullptr);
    ~StateMachine();

    void setConfigLoader(ConfigLoader *loader);
    void setFrameHandler(FrameHandler *handler);
    void start(const QString &deviceType, const QString &command);
    void stop();

private slots:
    void onTimerTimeout();

private:
    ConfigLoader *m_configLoader;
    FrameHandler *m_frameHandler;
    QTimer *m_timer;
    QString m_deviceType;
    QString m_command;
    bool m_ackReceived;

    void handleResponse(const QByteArray &response);
};

#endif // STATEMACHINE_H