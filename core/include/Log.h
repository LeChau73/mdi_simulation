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
// Macro LOG_DEBUG tạo một instance FileLogger tạm thời để ghi vào một file cụ thể.
#define LOG_DEBUG(name) (FileLogger(name))

class FileLogger
{
public:
    // Constructor 1: Nhận một đường dẫn file đầy đủ. Được dùng bởi singleton và cho mục đích chung.
    // 'explicit' để ngăn việc chuyển đổi kiểu dữ liệu ngoài ý muốn.
    explicit FileLogger(const QString &filePath = QCoreApplication::applicationDirPath() + "/log.txt")
    {
        init(filePath);
        m_isRawHexFile = false; // Đây là file log thông thường
    }

    // Constructor 2: Nhận một chuỗi ký tự C-style (ví dụ: LOG_DEBUG("test")).
    // Điều này giải quyết sự không rõ ràng với constructor nhận QString và dùng để tạo các file log .hex.
    FileLogger(const char *name)
    {
        init(QCoreApplication::applicationDirPath() + "/Input/" + name + ".txt");
        m_isRawHexFile = true; // Đây là file hex thô
    }

    ~FileLogger()
    {
        // Đảm bảo stream được flush và file được đóng khi đối tượng bị hủy.
        // Chỉ thêm dòng mới cho các file log thông thường, không thêm cho file hex.
        if (!m_isRawHexFile)
        {
            stream << "\n";
        }
        stream.flush();
        file.close();
    }

    FileLogger &printLog()
    {
        // FIX: Phương thức này đã bị lỗi. Nó phải trả về instance hiện tại (*this),
        // không phải instance singleton, để LOG_DEBUG hoạt động đúng.
        stream << "\n";
        stream.flush();
        return *this;
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
    // Hàm init riêng tư để tránh lặp lại code mở file trong các constructor.
    void init(const QString &filePath)
    {
        file.setFileName(filePath);
        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            stream.setDevice(&file);
            stream.setEncoding(QStringConverter::Utf8);
        }
        else
        {
            qWarning() << "Cannot open log file:" << filePath;
        }
    }

    QFile file;
    QTextStream stream;
    bool m_isRawHexFile;

    // Ngăn việc copy hoặc tạo instance từ bên ngoài
    FileLogger(const FileLogger &) = delete;
    FileLogger &operator=(const FileLogger &) = delete;
};

#endif // LOG_H