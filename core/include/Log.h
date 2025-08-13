#include <QFile>
#include <QTextStream>
#include <QDateTime>

class FileLogger
{
public:
    explicit FileLogger(const QString &filePath)
    {
        file.setFileName(filePath);
        file.open(QIODevice::Append | QIODevice::Text);
        stream.setDevice(&file);
        stream.setCodec("UTF-8"); // UTF-8 cho tiếng Việt
        // Thêm timestamp đầu dòng
        stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] ";
    }

    ~FileLogger()
    {
        stream << "\n";
        stream.flush();
        file.close();
    }

    template <typename T>
    FileLogger &operator<<(const T &value)
    {
        stream << value;
        return *this;
    }

private:
    QFile file;
    QTextStream stream;
};

FileLogger("app_log.txt") << "Type:" << data.type << " | Value:" << data.value;
FileLogger("app_log.txt") << "Something happened, code:" << 123;

inline FileLogger fileLog(const QString &path = "app_log.txt")
{
    return FileLogger(path);
}
