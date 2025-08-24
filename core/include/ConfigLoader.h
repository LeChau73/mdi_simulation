#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QObject>
#include <QVariantMap>
#include <QJsonDocument>

class ConfigLoader : public QObject
{
    Q_OBJECT
public:
    explicit ConfigLoader(QObject *parent = nullptr);
    ~ConfigLoader();

    bool loadConfig(const QString &jsonPath);
    QVariantMap getFrameConfig(const QString &deviceType, const QString &command) const;
    QVariantMap getSequenceConfig(const QString &deviceType, const QString &command) const;
    QString getCsvUrl(const QString &deviceType, const QString &command) const; // Thêm phương thức mới

private:
    QVariantMap m_configData; // Lưu toàn bộ JSON
};

#endif // CONFIGLOADER_H