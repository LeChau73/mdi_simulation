#include "FrameHandler.h"
#include <QDebug>
#include <QByteArray>
#include "Log.h"
#include <QFile>
#include <QProcess>
#include <QCoreApplication>

#define DEBUG

FrameHandler::FrameHandler(QObject *parent) : QObject(parent), m_configLoader(nullptr), m_csvParser(nullptr) {}

FrameHandler::~FrameHandler() {}

void FrameHandler::setConfigLoader(ConfigLoader *loader)
{
    m_configLoader = loader;
}

void FrameHandler::setCsvParser(CsvParser *parser)
{
    m_csvParser = parser;
}

bool FrameHandler::loadData(const QString &deviceType, const QString &command, const QString &csvPath)
{
    if (m_configLoader)
    {
        m_configLoader->getFrameConfig(deviceType, command); // Giả sử load đã gọi
    }
    if (m_csvParser)
    {
        return m_csvParser->loadCsv(csvPath);
    }
    return false;
}

QByteArray FrameHandler::buildFrame(const QString &deviceType, const QString &command)
{
    if (!m_configLoader || !m_csvParser)
    {
        qWarning() << "ConfigLoader or CsvParser not set!";
        return QByteArray();
    }

    QVariantMap frameConfig = m_configLoader->getFrameConfig(deviceType, command);
    QList<DataCsv1> dataStructure = m_csvParser->getDataStructure();
    QByteArray frame;

    // 1. Thêm startByte nếu có
    if (frameConfig.contains("startByte"))
    {
        frame.append(QByteArray::fromHex(frameConfig.value("startByte").toString().remove("0x").toUtf8()));
    }

    // 2. Thêm header và các trường đặc thù với delimiter
    QByteArray delimiter = QByteArray::fromHex(frameConfig.value("delimiter").toString().remove("0x").toUtf8());
    bool firstField = true;

    if (frameConfig.contains("header"))
    {
        frame.append(frameConfig.value("header").toString().toUtf8());
        firstField = false;
    }
    if (frameConfig.contains("messageLength"))
    {
        if (!firstField)
            frame.append(delimiter); // Thêm delimiter nếu không phải trường đầu tiên
        int msgLength = frameConfig.value("messageLength").toInt();
        frame.append(QString::number(msgLength).rightJustified(4, '0').toUtf8());
        firstField = false;
    }
    if (frameConfig.contains("fieldCount"))
    {
        if (!firstField)
            frame.append(delimiter); // Thêm delimiter nếu không phải trường đầu tiên
        int fieldCount = frameConfig.value("fieldCount").toInt();
        frame.append(QString::number(fieldCount).rightJustified(3, '0').toUtf8());
        firstField = false;
    }
    if (frameConfig.contains("dataStart"))
    {
        frame.append(QByteArray::fromHex(frameConfig.value("dataStart").toString().remove("0x").toUtf8()));
    }

    // 3. Xây dựng body, chỉ thêm trường có send = 1
    QByteArray paddingByte = QByteArray::fromHex(frameConfig.value("paddingByte").toString().remove("0x").toUtf8());
    QByteArray invalidPadding = frameConfig.contains("invalidPadding")
                                    ? QByteArray::fromHex(frameConfig.value("invalidPadding").toString().remove("0x").toUtf8())
                                    : paddingByte;

    QByteArray body;
    firstField = true;
    for (const DataCsv1 &data : dataStructure)
    {
        if (data.send == 1)
        {                                                                           // Chỉ xử lý trường có send = 1
            QString value = m_userInputs.value(data.name, data.value).remove("\""); // Loại bỏ nháy ké
            QByteArray valueData = value.toUtf8();
            int paddingSize = data.byteLimit - valueData.size();

            if (paddingSize > 0)
            {
                if (value.isEmpty() || value == "-" || value == "---")
                {
                    valueData = QByteArray(data.byteLimit, invalidPadding.at(0)); // 0x2d cho CI500
                }
                else
                {
                    valueData.append(QByteArray(paddingSize, paddingByte.at(0)));
                }
            }
            else if (paddingSize < 0)
            {
                valueData = valueData.left(data.byteLimit);
            }

            if (!firstField)
            {
                body.append(delimiter); // Thêm delimiter trước trường, trừ trường đầu tiên
            }
            body.append(valueData);
            firstField = false;
        }
        else
        {
            // Điền padding cho trường send = 0 nếu preserveStructure = true
            if (frameConfig.value("preserveStructure", false).toBool())
            {
                QByteArray paddingData = QByteArray(data.byteLimit, paddingByte.at(0));
                if (!firstField)
                {
                    body.append(delimiter);
                }
                body.append(paddingData);
                firstField = false;
            }
        }
    }
    frame.append(body);

    // 4. Thêm checksum nếu có
    if (frameConfig.contains("checksumType"))
    {
        QString checksumType = frameConfig.value("checksumType").toString();
        QByteArray dataForChecksum = body; // Dữ liệu sau header
        if (checksumType == "XOR")
        {
            char checksum = 0x00;
            for (char byte : dataForChecksum)
                checksum ^= byte;
            frame.append(QByteArray(1, checksum));
        }
        else if (checksumType == "SUM")
        {
            quint16 checksum = 0;
            for (char byte : dataForChecksum)
                checksum += static_cast<unsigned char>(byte);
            // Sử dụng QString để định dạng hex, sau đó chuyển thành QByteArray
            QString checksumStr = QString::number(checksum % 256, 16).rightJustified(2, '0');
            frame.append(checksumStr.toUtf8());
        }
    }

    // 5. Thêm endByte
    if (frameConfig.contains("endByte"))
    {
        frame.append(QByteArray::fromHex(frameConfig.value("endByte").toString().remove("0x").toUtf8()));
    }
    LOG_DEBUG("log") << "SIZE = " << frame.size();
    // LOG_DEBUG(deviceType.toStdString().c_str()) << frame.toHex(); // Dòng cũ: in ra chuỗi hex liền nhau, ví dụ: "4d4953..."

    // Cách mới: In ra giá trị số (thập phân) của từng byte, cách nhau bởi dấu cách.
    // Ví dụ: byte 0x4D sẽ được in là "77", 0x49 là "73". Kết quả: "77 73 83..."
    QString numericString;
    for (int i = 0; i < frame.size(); ++i)
    {
        // Chuyển byte thành giá trị số không dấu (0-255) và nối vào chuỗi
        numericString.append(QString::number(static_cast<unsigned char>(frame.at(i))));
        numericString.append(' '); // Thêm dấu cách để dễ đọc
    }
    LOG_DEBUG(deviceType.toStdString().c_str()) << numericString.trimmed(); // .trimmed() để xóa dấu cách thừa ở cuối

    // 6. In frame dưới dạng dễ đọc thay vì chỉ hex
    QString readableFrame;
    int delimiterPos = frame.indexOf(delimiter);
    if (delimiterPos != -1)
    {
        QByteArray header = frame.left(delimiterPos); // Phần trước delimiter (header, messageLength, fieldCount)
        readableFrame = QString(header).trimmed();    // Chuyển header thành chuỗi ASCII

        QByteArray data = frame.mid(delimiterPos + delimiter.size()); // Phần dữ liệu sau header
                                                                      // Sử dụng byte đầu tiên của delimiter làm ký tự split
        char delimiterChar = delimiter.isEmpty() ? '\0' : delimiter.at(0);
        QList<QByteArray> fields = data.split(delimiterChar); // Tách các trường

        for (int i = 0; i < fields.size(); ++i)
        {
            QString field = QString(fields[i]).trimmed(); // Chuyển thành chuỗi ASCII
            if (!field.isEmpty())
            {
                if (i > 0)
                    readableFrame += ", "; // Thêm comma và space cho định dạng
                readableFrame += field;
                if (i % 2 == 1 || field.length() > 10)
                    readableFrame += "\n"; // Xuống dòng sau mỗi 2 trường hoặc trường dài
            }
        }
    }
    else
    {
        readableFrame = QString(frame).trimmed(); // Nếu không có delimiter, in toàn bộ
    }

    // Mấy cái headder chưa có delimit
#ifdef DEBUG

    qDebug() << "Built frame (readable):" << readableFrame;
    LOG_INFO() << "Built frame (readable):" << readableFrame;
    LOG_DEBUG("PB_parse") << readableFrame;
    qDebug() << "Built frame (hex):" << frame.toHex(); // Giữ log hex để debug thô

    // --- Gọi script Python để xử lý frame ---
    QProcess pythonProcess;
    QString pythonExecutable = "python ";
    QString workDir = QCoreApplication::applicationDirPath() + "/Input/";

    // Ghi frame vào file input để script Python xử lý
    QString inputFilePath = workDir + deviceType + ".txt";
    QFile inputFile(inputFilePath);
    if (inputFile.open(QIODevice::WriteOnly))
    {
        inputFile.write(frame);
        inputFile.close();
    }
    else
    {
        qWarning() << "Could not open file for writing:" << inputFilePath;
    }

    pythonProcess.setWorkingDirectory(workDir);

    QString scriptName;
    QStringList args;

    if (deviceType == "CI500")
    {
        scriptName = "parser.py";
        args << scriptName << (deviceType + ".txt") << "09" << "-o" << "../Output/" + (deviceType + ".csv");
    }
    else if (deviceType == "HSC-CFP")
    {
        scriptName = "parser.py";
        args << scriptName << (deviceType + ".txt") << "2c" << "-o" << "../Output/" + (deviceType + ".csv");
    }
    else if (deviceType == "INVOS5100C")
    {
        scriptName = "parse_INVOS.py";
        args << scriptName << (deviceType + ".txt") << "-d" << " {2,4}" << "-o" << "../Output/" + (deviceType + ".csv");
    }
    else if (deviceType == "PB")
    {
        scriptName = "parser.py";
        args << scriptName << (deviceType + ".txt") << "2c" << "-o" << "../Output/" + (deviceType + ".csv");
    }

    if (!scriptName.isEmpty())
    {
        LOG_INFO() << "Calling Python script:" << pythonExecutable << args.join(" ");

        // Bắt đầu tiến trình và đợi nó kết thúc (với timeout 30 giây)
        pythonProcess.start(pythonExecutable, args);
        if (!pythonProcess.waitForFinished(30000))
        {
            LOG_INFO() << "Python script timed out or failed to start:" << pythonProcess.errorString();
        }
        else
        {
            // Đọc output từ script để debug
            LOG_INFO() << "Python script stdout:" << QString::fromLocal8Bit(pythonProcess.readAllStandardOutput());
            LOG_INFO() << "Python script stderr:" << QString::fromLocal8Bit(pythonProcess.readAllStandardError());
        }
    }

#endif

    return frame;
}
