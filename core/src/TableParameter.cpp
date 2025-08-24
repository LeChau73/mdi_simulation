#include "../include/TableParameter.h"
#include <QFile>
#include <QTextStream>

TableParameter::TableParameter(QObject *parent) : QObject(parent), m_model(new QStandardItemModel(this))
{
    m_DeviceManager = DeviceManager::GetInstance();

    m_FileDataLoader = new FileDataLoader(this);

    deviceType = m_DeviceManager->currentDeviceType();
    command = m_DeviceManager->currentCommand();
}

void TableParameter::getDataForTable()
{
    QList<QStringList> dataCsv;
    dataCsv = m_FileDataLoader->getCsvDefaultValue(m_DeviceManager->currentPathDevice());
}

QStandardItemModel *TableParameter::loadTableModel()
{
    QList<QStringList> dataCsv;
    LOG_INFO() << m_DeviceManager->currentPathDevice();
    dataCsv = m_FileDataLoader->getCsvDefaultValue(m_DeviceManager->currentPathDevice());
    csvData.clear();
    m_model->clear();
    m_model->setColumnCount(3);
    m_model->setHorizontalHeaderLabels(QStringList() << "Name Parameter" << "Value" << "Send");
    for (QStringList &para : dataCsv)
    {
        // Add data lấy từ csv vào thùng chứa
        DataCsv data;
        QString name = para[0];
        data.value = para[1];
        data.send = para[2].toInt();
        data.type = para[3];
        data.options = para[4].split(",");
        data.byteLimit = para[5].toInt();
        csvData.append(data);

        QList<QStandardItem *> row;
        row << new QStandardItem(name);
        row << new QStandardItem(para[1]);
        row << new QStandardItem(para[2]);
        // append vào thì hàng sẽ tự động +1
        m_model->appendRow(row);

        // vị trí index hiện tại dựa trên ( row và col )
        QModelIndex nameIndex = m_model->index(m_model->rowCount() - 1, 0);

        // add medata cho từng parameter
        m_model->setData(nameIndex, data.type, Qt::UserRole + 1); // add vào tất cả các option để tham chiếu

        if (data.type == "option" || data.type == "parameter")
        {
            m_model->setData(nameIndex, QVariant::fromValue(data.options), Qt::UserRole + 2); // add vào tất cả các option để tham chiếu
        }

        m_model->setData(nameIndex, data.byteLimit, Qt::UserRole + 3); // add vào tất cả các option để tham chiếu
    }
    return m_model;
}

void TableParameter::valueTableChanged(const int &index)
{
    QModelIndex valueIndex, sendIndex;
    QString dataChanged;
    int sendChanged;

    valueIndex = m_model->index(index, 1);
    sendIndex = m_model->index(index, 2);
    dataChanged = m_model->data(valueIndex, Qt::EditRole).toString();
    sendChanged = m_model->data(sendIndex, Qt::EditRole).toInt();

    if (csvData[index].value != dataChanged)
    {
        csvData[index].value = dataChanged;

        LOG_INFO() << "index : " << index << "  value changed: " << dataChanged;
    }
    else if (csvData[index].send != sendChanged)
    {
        csvData[index].send = sendChanged;
        LOG_INFO() << "index : " << index << "  send: " << sendChanged;
    }
}

void TableParameter::loadFileCsvFrom(const QString &fileCsvPath)
{
    // TODO: Xử lý loadfile lên
    LOG_INFO() << fileCsvPath;
}
