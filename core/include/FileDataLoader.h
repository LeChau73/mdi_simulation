#ifndef FILEDATALOADER_H
#define FILEDATALOADER_H

#include <QObject>
#include <QJsonObject>
#include <QStandardItemModel>
#include "../common/Log.h"

class FileDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit FileDataLoader(QObject *parent = nullptr) : QObject(parent) {}

    QList<QStringList> getCsvDefaultValue(const QString &defaultsPath);

    void updateCsvData(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

signals:
    void loadingError(const QString &errorMessage);

private:
    QJsonObject loadJsonConfig(const QString &deviceType, const QString &command);
    QString getCsvValue(const QString &deviceType, const QString &command, const QString &paramName);
    int getCsvDefaultSend(const QString &deviceType, const QString &command, const QString &paramName); // Thêm phương thức mới
    int getCsvSend(const QString &deviceType, const QString &command, const QString &paramName);
    bool writeCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

    QHash<QString, QJsonObject> m_jsonCache;
    QHash<QString, QString> m_defaultValueCache; // Cache cho default values
};

#endif // FILEDATALOADER_H
