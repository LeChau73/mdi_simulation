#include "../include/FileDataLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// INFO: tách dấu , và /" ở lần đầu gặp,ở trong thì thêm vào
// "NIV,INVASIVE" thì nó add và 1 list [ NIV,INVASIVE ]
QStringList parseCsvLine(const QString &line, QChar delimiter = ',')
{
    QStringList result;
    QString current;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i)
    {
        QChar ch = line[i];

        if (ch == '\"')
        {
            // Nếu đang trong quotes và gặp "", tức là dấu " escape
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '\"')
            {
                current += '\"';
                ++i; // bỏ qua ký tự thứ 2
            }
            else
            {
                inQuotes = !inQuotes; // đảo trạng thái trong/ngoài dấu nháy
            }
        }
        else if (ch == delimiter && !inQuotes)
        {
            result.append(current);
            current.clear();
        }
        else
        {
            current += ch;
        }
    }
    result.append(current);
    return result;
}

// Lấy giá trị
QList<QStringList> FileDataLoader::getCsvDefaultValue(const QString &defaultsPath)
{
    FileLogger() << defaultsPath;
    QFile defaultsFile(defaultsPath);
    QList<QStringList> listData;
    if (defaultsFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&defaultsFile);
        in.setEncoding(QStringConverter::Utf8);
        in.readLine(); // Bỏ qua header
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = parseCsvLine(line);
            listData.append(fields);
        }
        defaultsFile.close();
    }
    return listData; // Trả về rỗng nếu không tìm thấy
}

int FileDataLoader::getCsvDefaultSend(const QString &deviceType, const QString &command, const QString &paramName)
{
    QString defaultsPath = QCoreApplication::applicationDirPath() + "/config/data_csv/" + deviceType + "_" + command + "_defaults.csv";
    QFile defaultsFile(defaultsPath);
    if (defaultsFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&defaultsFile);
        in.readLine(); // Bỏ qua header
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 3 && fields[0] == paramName)
            {
                return fields[2].toInt(); // Send
            }
        }
        defaultsFile.close();
    }
    return 0; // Mặc định Send là 0 nếu không tìm thấy
}

QJsonObject FileDataLoader::loadJsonConfig(const QString &deviceType, const QString &command)
{
    QJsonObject config;
    QString jsonPath = QCoreApplication::applicationDirPath() + "/config/device_config/" + deviceType + ".json";
    qDebug() << jsonPath;
    QFile jsonFile(jsonPath);
    if (jsonFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
        if (!doc.isNull() && doc.isObject())
        {
            QJsonObject jsonObj = doc.object();
            if (jsonObj.contains(command))
            {
                config = jsonObj[command].toObject();
            }
        }
        jsonFile.close();
    }
    return config;
}

QString FileDataLoader::getCsvValue(const QString &deviceType, const QString &command, const QString &paramName)
{
    QString csvPath = QCoreApplication::applicationDirPath() + "/data/" + deviceType + "_" + command + "_data.csv";
    QFile csvFile(csvPath);
    if (csvFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&csvFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 3 && fields[1] == paramName) // Fix: Thêm check size
                return fields[2];                             // Cột 2 là Value
        }
        csvFile.close();
    }
    return QString();
}

bool FileDataLoader::writeCsvValue(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send)
{
    QString csvPath = QCoreApplication::applicationDirPath() + "/config/data_csv/" + deviceType + "_" + command + "_data.csv";
    QFile csvFile(csvPath);
    if (!csvFile.open(QIODevice::ReadWrite))
    {
        qWarning() << "Cannot open CSV file for reading:" << csvPath;
        return false;
    }

    QStringList lines;
    QTextStream in(&csvFile);
    bool found = false;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() >= 4 && fields[1] == paramName) // Fix: Thêm check size
        {
            fields[2] = value;                         // Cập nhật Value
            fields[3] = QString::number(send ? 1 : 0); // Cập nhật Send
            found = true;
        }
        lines.append(fields.join(","));
    }
    csvFile.close();

    if (!found)
    {
        qWarning() << "Parameter" << paramName << "not found in CSV";
        return false;
    }

    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "Cannot open CSV file for writing:" << csvPath;
        return false;
    }

    QTextStream out(&csvFile);
    for (const QString &line : lines)
    {
        out << line << "\n";
    }
    csvFile.close();
    return true;
}

void FileDataLoader::updateCsvData(const QString &deviceType, const QString &command, const QString &paramName, const QString &value, bool send)
{
    if (writeCsvValue(deviceType, command, paramName, value, send))
    {
        qDebug() << "Updated CSV for" << paramName << "with value" << value << "and send" << send;
    }
    else
    {
        emit loadingError("Failed to update CSV for " + paramName);
    }
}
