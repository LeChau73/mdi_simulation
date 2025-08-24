#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include "FrameHandler.h"

class DataModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStandardItemModel *model READ model CONSTANT)

public:
    explicit DataModel(QObject *parent = nullptr);
    ~DataModel();

    void setFrameHandler(FrameHandler *handler);
    QStandardItemModel *model() const;

public slots:
    void updateValue(const QString &name, const QString &value);

private:
    FrameHandler *m_frameHandler;
    QStandardItemModel *m_model;
};

#endif // DATAMODEL_H