#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QJsonObject>
#include "DeviceBase.h"

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager() override;

    // TODO: factory device
    QString currentDeviceType() const;
    void setCurrentDeviceType(const QString &deviceType);

    QString currentCommand() const;
    void setCurrentCommand(const QString &command);

    QString currentPathDevice() const;
    void setCurrentPathDevice(const QString &currentPath);

    QString buildFrame(const QJsonObject &params);

    static DeviceManager *GetInstance();

private:
    QString m_currentDeviceType;
    QString m_command;
    QString m_pathFileCsv;

    std::unique_ptr<DeviceBase> m_currentDevice;
};

#endif // DEVICEMANAGER_H