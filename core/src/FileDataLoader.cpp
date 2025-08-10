#include "../include/FileDataLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QString FileDataLoader::getCsvDefaultValue(const QString &deviceType, const QString &command, const QString &paramName)
{
    QString defaultsPath = QCoreApplication::applicationDirPath() + "/data/" + deviceType + "_" + command + "_defaults.csv";
    qDebug() << defaultsPath;
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
                return fields[1]; // DefaultValue
            }
        }
        defaultsFile.close();
    }
    return QString(); // Trả về rỗng nếu không tìm thấy
}

int FileDataLoader::getCsvDefaultSend(const QString &deviceType, const QString &command, const QString &paramName)
{
    QString defaultsPath = QCoreApplication::applicationDirPath() + "/data/" + deviceType + "_" + command + "_defaults.csv";
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

QStandardItemModel *FileDataLoader::loadTableModel(const QString &deviceType, const QString &command)
{
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(3);                                                                 // Giảm từ 4 xuống 3 cột
    model->setHorizontalHeaderLabels(QStringList() << "Name Parameter" << "Value" << "Send"); // Bỏ cột STT

    QJsonObject config = loadJsonConfig(deviceType, command);
    if (config.isEmpty())
    {
        qWarning() << "Failed to load JSON config for" << deviceType << command;
        return model;
    }

    for (auto param : config["params"].toArray())
    {
        QJsonObject paramObj = param.toObject();
        QString name = paramObj["name"].toString();
        QString defaultValue = getCsvDefaultValue(deviceType, command, name);
        QString byteLimit = QString::number(paramObj["byteLimit"].toInt());
        QString value = getCsvValue(deviceType, command, name);
        int send = getCsvDefaultSend(deviceType, command, name);

        name += "(" + byteLimit + "byte" + ")";

        QList<QStandardItem *> row;
        row << new QStandardItem(name); // Bỏ cột STT, bắt đầu với name
        row << new QStandardItem(value.isEmpty() ? defaultValue : value);
        row << new QStandardItem(QString::number(send));
        model->appendRow(row);

        // Fix: Lưu metadata vào item đầu tiên thay vì index(rowCount-1, 0)
        QModelIndex nameIndex = model->index(model->rowCount() - 1, 0);
        model->setData(nameIndex, paramObj["type"].toString(), Qt::UserRole + 1);
        if (paramObj["type"].toString() == "option")
        {
            model->setData(nameIndex, paramObj["options"].toVariant(), Qt::UserRole + 2);
        }
        else if (paramObj["type"].toString() == "parameter")
        {
            model->setData(nameIndex, paramObj["state"].toVariant(), Qt::UserRole + 2);
        }
    }
    return model;
}

QJsonObject FileDataLoader::loadJsonConfig(const QString &deviceType, const QString &command)
{
    QJsonObject config;
    QString jsonPath = QCoreApplication::applicationDirPath() + "/config/" + deviceType + ".json";
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
    QString csvPath = QCoreApplication::applicationDirPath() + "/data/" + deviceType + "_" + command + "_data.csv";
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
