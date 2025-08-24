#include <QFile>
#include <QTextStream>
#include <QDateTime>

class FileLogger
{
public:
    explicit FileLogger(const QString &filepath);
    {
    }

private:
    QFile file;
    QTextStream stream;
}