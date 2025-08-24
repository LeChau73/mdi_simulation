

#include <QObject>
#include <QJsonObject>
#include "../include/DeviceManager.h"
#include "../include/DeviceBase.h"
#include <QCoreApplication>

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    // TODO: nhảy vào file.config read devive và path ra
    m_currentDeviceType = "PB";
    m_command = "MISCF";
    m_pathFileCsv = QCoreApplication::applicationDirPath() + "/config/data_csv/" + m_currentDeviceType + "_" + m_command + "_defaults.csv";
}

DeviceManager::~DeviceManager() {}

QString DeviceManager::currentPathDevice() const
{
    return m_pathFileCsv;
}

void DeviceManager::setCurrentPathDevice(const QString &currentPath)
{
    this->m_pathFileCsv = currentPath;
}

QString DeviceManager::currentDeviceType() const
{
    return m_currentDeviceType;
}

DeviceManager *DeviceManager::GetInstance()
{
    static DeviceManager *s_instance = new DeviceManager();
    return s_instance;
}

QString DeviceManager::currentCommand() const
{
    return m_command;
}

void DeviceManager::setCurrentCommand(const QString &command)
{
}