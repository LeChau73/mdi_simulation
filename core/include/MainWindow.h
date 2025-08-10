#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include "FileDataLoader.h"

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = nullptr);
    Q_INVOKABLE QStandardItemModel *loadTableModel(const QString &deviceType, const QString &command);

public slots:
    void updateCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);

signals:
    void deviceChanged(const QString &newDevice, const QString &newCommand);

private:
    FileDataLoader *m_fileLoader;
};

#endif // MAINWINDOW_H