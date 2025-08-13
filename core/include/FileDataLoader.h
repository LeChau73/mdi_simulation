#ifndef FILEDATALOADER_H
#define FILEDATALOADER_H

#include <QObject>
#include <QJsonObject>
#include <QStandardItemModel>

struct DataCsv
{
    QString value;
    QString send;
    QString type;
    QStringList options;
    int byteLimit;
};

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
    QHash<QString, DataCsv> csvData;
    QList<QString> orderList; // Lưu thứ tự các name
    QJsonObject loadJsonConfig(const QString &deviceType, const QString &command);
    QString getCsvValue(const QString &deviceType, const QString &command, const QString &paramName);

    bool getCsvDefaultValue(const QString &deviceType, const QString &command);

    int getCsvDefaultSend(const QString &deviceType, const QString &command, const QString &paramName); // Thêm phương thức mới

    int getCsvSend(const QString &deviceType, const QString &command, const QString &paramName);
    bool writeCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

    QHash<QString, QJsonObject> m_jsonCache;
    QHash<QString, QPair<QString, bool>> m_defaultValueCache; // Cache cho default values [key] [[value] [send]]
};

#endif // FILEDATALOADER_H