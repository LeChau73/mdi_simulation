#include "ConfigLoader.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <Log.h>

ConfigLoader::ConfigLoader(QObject *parent) : QObject(parent) {}

ConfigLoader::~ConfigLoader() {}

bool ConfigLoader::loadConfig(const QString &jsonPath)
{
    QFile file(jsonPath);
    LOG_INFO() << "jsonPath" << jsonPath;
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open JSON:" << jsonPath;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (doc.isNull() || !doc.isObject())
    {
        qWarning() << "Invalid JSON format:" << jsonPath;
        return false;
    }
    m_configData = doc.object().toVariantMap();
    qDebug() << "Loaded config from JSON.";
    return true;
}

QVariantMap ConfigLoader::getFrameConfig(const QString &deviceType, const QString &command) const
{
    QVariantMap devices = m_configData.value("devices").toMap();
    QVariantMap commands = devices.value(deviceType).toMap().value("commands").toMap();
    return commands.value(command).toMap().value("frame").toMap();
}

QVariantMap ConfigLoader::getSequenceConfig(const QString &deviceType, const QString &command) const
{
    QVariantMap devices = m_configData.value("devices").toMap();
    QVariantMap commands = devices.value(deviceType).toMap().value("commands").toMap();
    return commands.value(command).toMap().value("sequence").toMap();
}

QString ConfigLoader::getCsvUrl(const QString &deviceType, const QString &command) const
{
    QVariantMap devices = m_configData.value("devices").toMap();
    QVariantMap commands = devices.value(deviceType).toMap().value("commands").toMap();
    return commands.value(command).toMap().value("csvPath").toString();
}