#ifndef LOG_H
#define LOG_H
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDateTime>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QList>

// Macro để chèn thông tin file và dòng
#define LOG_INFO() (FileLogger::instance().printLog() << "[" << __FILE__ << " : " << __LINE__ << "] ")

class FileLogger
{
public:
    explicit FileLogger(const QString &filePath = QCoreApplication::applicationDirPath() + "log.txt")
    {
        file.setFileName(filePath);
        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            stream.setDevice(&file);
            stream.setEncoding(QStringConverter::Utf8);
        }
        else
        {
            qDebug() << "Cannot find log";
        }
    }

    ~FileLogger()
    {
        stream << "\n";
        stream.flush();
        file.close();
    }

    FileLogger &printLog()
    {
        stream << "\n";
        stream.flush();
        return FileLogger::instance();
    }

    static FileLogger &instance()
    {

        static FileLogger logger;
        return logger;
    }

    template <typename T>
    FileLogger &operator<<(const T &value)
    {
        stream << value;
        stream.flush(); // Flush ngay sau khi ghi
        return *this;
    }

    template <typename T>
    FileLogger &operator<<(const QList<T> &list)
    {
        stream << "[ ";
        for (const T &item : list)
        {
            stream << item << " ";
        }
        stream << "]";
        stream.flush(); // Flush ngay sau khi ghi
        return *this;
    }

private:
    QFile file;
    QTextStream stream;

    // Ngăn việc copy hoặc tạo instance từ bên ngoài
    FileLogger(const FileLogger &) = delete;
    FileLogger &operator=(const FileLogger &) = delete;
};

#endif