#ifndef FILEDATALOADER_H
#define FILEDATALOADER_H

#include <QObject>
#include <QJsonObject>
#include <QStandardItemModel>

class FileDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit FileDataLoader(QObject *parent = nullptr) : QObject(parent) {}

    QStandardItemModel *loadTableModel(const QString &deviceType, const QString &command);

    void updateCsvData(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

signals:
    void loadingError(const QString &errorMessage);

private:
    QJsonObject loadJsonConfig(const QString &deviceType, const QString &command);
    QString getCsvValue(const QString &deviceType, const QString &command, const QString &paramName);
    QString getCsvDefaultValue(const QString &deviceType, const QString &command, const QString &paramName);
    int getCsvDefaultSend(const QString &deviceType, const QString &command, const QString &paramName); // Thêm phương thức mới
    int getCsvSend(const QString &deviceType, const QString &command, const QString &paramName);
    bool writeCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

    QHash<QString, QJsonObject> m_jsonCache;
    QHash<QString, QString> m_defaultValueCache; // Cache cho default values
};

#endif // FILEDATALOADER_H