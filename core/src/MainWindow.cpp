#include "MainWindow.h"
#include <QDebug>
#include "Log.h"

/*
MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    m_DeviceManager = DeviceManager::GetInstance();
    m_TableParameter = new TableParameter(this);
    m_fileLoader = new FileDataLoader(this);


}
*/

MainWindow::MainWindow(QObject *parent) : QObject(parent),
                                          m_fileLoader(new FileDataLoader(this)),
                                          m_TableParameter(new TableParameter(this)),
                                          m_DeviceManager(DeviceManager::GetInstance()),
                                          m_configLoader(new ConfigLoader(this)),
                                          m_csvParser(new CsvParser(this)),
                                          m_frameHandler(new FrameHandler(this)),
                                          m_stateMachine(new StateMachine(this)),
                                          m_dataModel(new DataModel(this))
{
    initialize();
}

void MainWindow::initialize()
{
    m_frameHandler->setConfigLoader(m_configLoader);
    m_frameHandler->setCsvParser(m_csvParser);
    m_stateMachine->setConfigLoader(m_configLoader);
    m_stateMachine->setFrameHandler(m_frameHandler);
    m_dataModel->setFrameHandler(m_frameHandler);
    // Kết nối QML với MainWindow nếu cần
}

QStandardItemModel *MainWindow::loadTableParameter()
{
    LOG_INFO() << "fileCsvChanged in C++";
    return m_TableParameter->loadTableModel();
}

void MainWindow::updateCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send)
{
    m_fileLoader->updateCsvData(deviceType, command, paramName, value, send); // Gọi phương thức trong FileDataLoader
}

void MainWindow::valueTableChanged(const int &index)
{
    m_TableParameter->valueTableChanged(index);
}

void MainWindow::loadFileCsvFrom(const QString &fileCsvPath)
{
    LOG_INFO() << fileCsvPath;
    m_DeviceManager->setCurrentPathDevice(fileCsvPath);
    LOG_INFO() << "Prepare emit file";
    emit fileCsvChanged();
}

void MainWindow::loadDevice(const QString &deviceType, const QString &command, QString pathJson)
{
    if (m_configLoader->loadConfig(pathJson + "/config.json"))
    {
        QString csvPath = m_configLoader->getCsvUrl(deviceType, command);
        if (m_csvParser->loadCsv(csvPath))
        {
            m_frameHandler->loadData(deviceType, command, csvPath);
            // m_dataModel->updateModel();
            m_stateMachine->start(deviceType, command);
            qDebug() << "Loaded device:" << deviceType << command;
        }
    }
}

void MainWindow::onUserInput(const QMap<QString, QString> &values)
{
    // m_frameHandler->setUserInput(values);
    // m_dataModel->updateModel();
}