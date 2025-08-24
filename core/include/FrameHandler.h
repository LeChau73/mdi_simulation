#ifndef FRAMEHANDLER_H
#define FRAMEHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QMap>
#include "ConfigLoader.h"
#include "CsvParser.h"

class FrameHandler : public QObject {
    Q_OBJECT
public:
    explicit FrameHandler(QObject* parent = nullptr);
    ~FrameHandler();

    void setConfigLoader(ConfigLoader* loader);
    void setCsvParser(CsvParser* parser);
    bool loadData(const QString& deviceType, const QString& command, const QString& csvPath);
    //void setUserInput(const QString& name, const QString& value);
    QByteArray buildFrame(const QString& deviceType, const QString& command);

private:
    ConfigLoader* m_configLoader;
    CsvParser* m_csvParser;
    QMap<QString, QString> m_userInputs; // Ghi đè value từ người dùng
};

#endif // FRAMEHANDLER_H