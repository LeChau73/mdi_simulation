#include "DataModel.h"
#include <QDebug>

DataModel::DataModel(QObject *parent) : QObject(parent), m_frameHandler(nullptr), m_model(new QStandardItemModel(this))
{
    m_model->setHorizontalHeaderLabels(QStringList() << "name" << "value" << "send" << "type" << "options" << "byteLimit");
}

DataModel::~DataModel() {}

void DataModel::setFrameHandler(FrameHandler *handler)
{
    m_frameHandler = handler;
}

QStandardItemModel *DataModel::model() const
{
    return m_model;
}

void DataModel::updateValue(const QString &name, const QString &value)
{
    if (m_frameHandler)
    {
        //m_frameHandler->setUserInput(name, value);
        qDebug() << "Updated value for" << name << ":" << value;
    }
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        if (m_model->item(row, 0)->text() == name)
        {
            m_model->item(row, 1)->setText(value);
            break;
        }
    }
}