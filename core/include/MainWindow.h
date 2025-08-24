#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include "FileDataLoader.h"
#include <QStandardItemModel>
#include "DeviceManager.h"
#include "TableParameter.h"
#include "ConfigLoader.h"
#include "CsvParser.h"
#include "FrameHandler.h"
#include "StateMachine.h"
#include "DataModel.h"

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = nullptr);
    void initialize();

    void loadDevice(const QString &deviceType, const QString &command, QString pathJson);

public slots:
    void updateCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send);
    void valueTableChanged(const int &index);
    void loadFileCsvFrom(const QString &fileCsvPath);
    void onUserInput(const QMap<QString, QString> &values);
    Q_INVOKABLE QStandardItemModel *loadTableParameter();

signals:
    void fileCsvChanged();
    // void deviceChanged(const QString &newDevice, const QString &newCommand);

private:
    FileDataLoader *m_fileLoader;
    TableParameter *m_TableParameter;
    DeviceManager *m_DeviceManager;

    ConfigLoader *m_configLoader;
    CsvParser *m_csvParser;
    FrameHandler *m_frameHandler;
    StateMachine *m_stateMachine;
    DataModel *m_dataModel;
};

#endif // MAINWINDOW_H