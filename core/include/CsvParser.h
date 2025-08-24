#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QObject>
#include <QList>
#include <QStringList>

struct DataCsv1
{
    QString name;
    QString value; // Giá trị mặc định từ CSV
    int send;
    QString type;
    QStringList options;
    int byteLimit;
};

class CsvParser : public QObject
{
    Q_OBJECT
public:
    explicit CsvParser(QObject *parent = nullptr);
    ~CsvParser();

    bool loadCsv(const QString &csvPath);
    QList<DataCsv1> getDataStructure() const;

private:
    QList<DataCsv1> m_dataStructure;
};

#endif // CSVPARSER_H