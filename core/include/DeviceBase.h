#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <QObject>
#include <QString>
#include <QJsonObject>

class DeviceBase : public QObject
{
public:
    explicit DeviceBase(QObject *parent = nullptr);
    virtual ~DeviceBase() = default;

    virtual QString getFileCsv() = 0;
    void setPathFileCsv() const;
    void setDevice() const;
    void setCommand() const;

protected:
    QString m_deviceType;
    QString m_pathFileCsv;
    QString m_command;
};

class PBDevice : public DeviceBase
{
    Q_OBJECT
public:
    explicit PBDevice(QObject *parent = nullptr);
    ~PBDevice() override;

    // QString getFileCsv() override
};

#endif