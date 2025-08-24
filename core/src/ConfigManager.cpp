#include "ConfigManager.h"
#include "Log.h"
ConfigManager::ConfigManager(QObject *parent) : QObject(parent)
{
}

// Load config
void ConfigManager::loadFromFile(const QString &path)
{
    QSettings s(path, QSettings::IniFormat);
    QFile file(path);
    LOG_INFO() << "loadFromFile" << path;
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Error reading file. Please try again.";

        return;
    }

    m_device = s.value("device").toString();
    command = s.value("command").toString();
}