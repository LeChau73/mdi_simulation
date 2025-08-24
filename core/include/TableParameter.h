#ifndef TABLEPARAMETER_H
#define TABLEPARAMETER_H

#include <QObject>
#include <QStandardItemModel>
#include "DeviceManager.h"
#include "FileDataLoader.h"
#include "Log.h"

struct DataCsv
{
    QString value;
    int send;
    QString type;
    QStringList options;
    int byteLimit;
};

class TableParameter : public QObject
{

public:
    explicit TableParameter(QObject *parent = nullptr);
    virtual ~TableParameter() = default;
    // Hiện tại tất cả device sẽ vào đây để load table
    // Nếu khác thì tách mỗi device sau
    QStandardItemModel *loadTableModel();
    void getDataForTable();
    void valueTableChanged(const int &index);
    void loadFileCsvFrom(const QString &fileCsvPath);

private:
    QList<DataCsv> csvData;

    QStandardItemModel *m_model;
    QString deviceType;
    QString command;

    DeviceManager *m_DeviceManager;
    FileDataLoader *m_FileDataLoader;
};

#endif