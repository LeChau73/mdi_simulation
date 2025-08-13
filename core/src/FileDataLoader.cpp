#include "../include/FileDataLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

bool FileDataLoader::getCsvDefaultValue(const QString &deviceType, const QString &command)
{
    // SPONT_TYPE,"",1,option,"PS,TC,VS,PA",6
    // Trigger_set,"V-Trig",1,option,"-TRIG,P-TRIG",6
    QString defaultsPath = QCoreApplication::applicationDirPath() + "/data/" + deviceType + "_" + command + "_defaults.csv";
    qDebug() << defaultsPath;
    QFile defaultsFile(defaultsPath);
    QList<QStringList> dataCSV;

    if (defaultsFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&defaultsFile);
        in.setEncoding(QStringConverter::Utf8);
        in.readLine(); // Bỏ qua header

        while (!in.atEnd())
        {
            QString line = in.readLine();
            DataCsv data;
            QStringList fields = parseCsvLine(line);
            QString name = fields[0];
            data.value = fields[1];
            data.send = fields[2];
            data.type = fields[3];
            data.options = fields[4].split(",");
            data.byteLimit = fields[5].toInt();
            csvData[name] = data;
            orderList.append(name);

            qDebug() << name;
            qDebug() << data.options;
        }
        defaultsFile.close();
    }
    return 1; // Trả về rỗng nếu không tìm thấy
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

    bool ok = getCsvDefaultValue(deviceType, command);
    if (!ok)
    {
        qWarning() << "Failed to load JSON config for" << deviceType << command;
        return model;
    }
    QHashIterator<QString, DataCsv> it(csvData);
    for (const QString &name : orderList)
    {
        const DataCsv &data = csvData[name];
        qDebug().noquote() << "-------------------------------";
        qDebug().noquote() << "Name      :" << name;
        qDebug().noquote() << "Value     :" << data.value;
        qDebug().noquote() << "Send      :" << data.send;
        qDebug().noquote() << "Type      :" << data.type;
        qDebug().noquote() << "Options   :" << data.options.join(", ");
        qDebug().noquote() << "ByteLimit :" << data.byteLimit;
    }
    qDebug().noquote() << "===============================";

    // for (auto param : config["params"].toArray())
    //{
    //     QJsonObject paramObj = param.toObject();
    //     QString name = paramObj["name"].toString();
    //     QString byteLimit = QString::number(paramObj["byteLimit"].toInt());
    //     QString value = getCsvValue(deviceType, command, name);
    //     int send = getCsvDefaultSend(deviceType, command, name);

    //    m_defaultValueCache.insert(name, qMakePair(defaultValue, static_cast<bool>(send)));

    //    qDebug() << m_defaultValueCache[name].first;
    //    qDebug() << m_defaultValueCache[name].second;

    //    name += "(" + byteLimit + "byte" + ")";

    //    QList<QStandardItem *> row;
    //    row << new QStandardItem(name); // Bỏ cột STT, bắt đầu với name
    //    row << new QStandardItem(value.isEmpty() ? defaultValue : value);
    //    row << new QStandardItem(QString::number(send));
    //    model->appendRow(row);

    //    // Fix: Lưu metadata vào item đầu tiên thay vì index(rowCount-1, 0)
    //    QModelIndex nameIndex = model->index(model->rowCount() - 1, 0);
    //    // Sets the role data for the item at index to value.
    //    // Set role + 1 cho "type": free ở index 3

    //    model->setData(nameIndex, paramObj["type"].toString(), Qt::UserRole + 1);   // truyền vào tất cả các loại free/option/parameter để so sánh
    //    model->setData(nameIndex, paramObj["byteLimit"].toInt(), Qt::UserRole + 3); // medata bytelimit cho parameter

    //    if (paramObj["type"].toString() == "option")
    //    {
    //        model->setData(nameIndex, paramObj["options"].toVariant(), Qt::UserRole + 2);
    //    }
    //    else if (paramObj["type"].toString() == "parameter")
    //    {
    //        model->setData(nameIndex, paramObj["state"].toVariant(), Qt::UserRole + 2);
    //    }
    //}
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
        // qWarning() << "Parameter" << paramName << "not found in CSV";
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
    m_defaultValueCache[paramName] = qMakePair(value, send);
    qDebug() << "Updated New Value" << paramName << "with value" << m_defaultValueCache[paramName].first << "and send" << m_defaultValueCache[paramName].second;
}
