#include "StateMachine.h"
#include <QDebug>

StateMachine::StateMachine(QObject *parent) : QObject(parent), m_configLoader(nullptr), m_frameHandler(nullptr), m_timer(new QTimer(this)), m_ackReceived(false)
{
    connect(m_timer, &QTimer::timeout, this, &StateMachine::onTimerTimeout);
}

StateMachine::~StateMachine() {}

void StateMachine::setConfigLoader(ConfigLoader *loader)
{
    m_configLoader = loader;
}

void StateMachine::setFrameHandler(FrameHandler *handler)
{
    m_frameHandler = handler;
}

void StateMachine::start(const QString &deviceType, const QString &command)
{
    m_deviceType = deviceType;
    m_command = command;

    QVariantMap sequence = m_configLoader->getSequenceConfig(deviceType, command);
    QString seqType = sequence.value("type").toString();
    int interval = sequence.value("interval").toInt();

    if (seqType == "auto-cycle")
    {
        m_timer->setInterval(interval);
        m_timer->start();
    }
    else if (seqType == "command-response")
    {
        QByteArray frame = m_frameHandler->buildFrame(deviceType, command);
        // Gửi frame và chờ phản hồi
        qDebug() << "Sent command frame:" << frame.toHex();
    }
}

void StateMachine::stop()
{
    m_timer->stop();
    m_ackReceived = false;
}

void StateMachine::onTimerTimeout()
{
    QByteArray frame = m_frameHandler->buildFrame(m_deviceType, m_command);
    qDebug() << "Auto-cycle sent frame:" << frame.toHex();
}

void StateMachine::handleResponse(const QByteArray &response)
{
    QVariantMap sequence = m_configLoader->getSequenceConfig(m_deviceType, m_command);
    bool ackNack = sequence.value("ackNack").toBool();
    if (ackNack)
    {
        QByteArray ackByte = QByteArray::fromHex(sequence.value("ackByte").toString().remove("0x").toUtf8());
        if (response == ackByte)
        {
            m_ackReceived = true;
            qDebug() << "ACK received";
        }
        else
        {
            qDebug() << "NACK received";
        }
    }
    else
    {
        qDebug() << "Received data:" << response.toHex();
    }
}