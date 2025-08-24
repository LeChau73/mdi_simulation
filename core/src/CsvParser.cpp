#include "CsvParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

CsvParser::CsvParser(QObject *parent) : QObject(parent) {}

CsvParser::~CsvParser() {}

bool CsvParser::loadCsv(const QString &csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open CSV file:" << csvPath;
        return false;
    }

    m_dataStructure.clear();
    QTextStream in(&file);
    QString header = in.readLine(); // Bỏ qua header
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() >= 6)
        {
            DataCsv1 data;
            data.name = fields[0].trimmed();
            data.value = fields[1].trimmed(); // Giá trị mặc định
            data.send = fields[2].toInt();
            data.type = fields[3].trimmed();
            data.options = fields[4].split(",");
            data.byteLimit = fields[5].toInt();
            m_dataStructure.append(data);
        }
    }
    file.close();

    qDebug() << "Loaded" << m_dataStructure.size() << "fields from CSV";
    return true;
}

QList<DataCsv1> CsvParser::getDataStructure() const
{
    return m_dataStructure;
}