#include "../include/MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    m_fileLoader = new FileDataLoader(this); // Instance duy nhất
}

QStandardItemModel *MainWindow::loadTableModel(const QString &deviceType, const QString &command)
{
    qDebug() << "MainWindow: LoadTableModel";
    return m_fileLoader->loadTableModel(deviceType, command);
}

void MainWindow::updateCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send)
{
    m_fileLoader->updateCsvData(deviceType, command, paramName, value, send); // Gọi phương thức trong FileDataLoader
}