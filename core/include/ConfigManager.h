#ifndef ConfigManager_H
#define ConfigManager_H

#include <QObject>
#include <QVariantMap>
#include <QSettings>
#include <QFile>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);
    void loadFromFile(const QString &path);
    QString m_device;
    QString command;

private:
};

#endif // ConfigManager_H
