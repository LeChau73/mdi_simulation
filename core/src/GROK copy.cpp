QByteArray FrameHandler::buildFrame(const QString &deviceType, const QString &command)
{
    if (!m_configLoader || !m_csvParser)
    {
        qWarning() << "ConfigLoader or CsvParser not set!";
        return QByteArray();
    }

    QVariantMap frameConfig = m_configLoader->getFrameConfig(deviceType, command);
    QList<DataCsv> dataStructure = m_csvParser->getDataStructure();
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
            frame.append(delimiter);
        int msgLength = frameConfig.value("messageLength").toInt();
        frame.append(QString::number(msgLength).rightJustified(4, '0').toUtf8());
        firstField = false;
    }
    if (frameConfig.contains("fieldCount"))
    {
        if (!firstField)
            frame.append(delimiter);
        int fieldCount = frameConfig.value("fieldCount").toInt();
        frame.append(QString::number(fieldCount).rightJustified(3, '0').toUtf8());
        firstField = false;
    }
    if (frameConfig.contains("dataStart"))
    {
        frame.append(QByteArray::fromHex(frameConfig.value("dataStart").toString().remove("0x").toUtf8()));
    }

    // 3. Xây dựng body, xử lý bit theo group thành 1 byte và padding đặc biệt cho CI500
    QByteArray paddingByte = QByteArray::fromHex(frameConfig.value("paddingByte").toString().remove("0x").toUtf8());
    QByteArray invalidPadding = frameConfig.contains("invalidPadding")
                                    ? QByteArray::fromHex(frameConfig.value("invalidPadding").toString().remove("0x").toUtf8())
                                    : QByteArray::fromHex("2d"); // Default 0x2D

    QByteArray body;
    firstField = true;

    int currentBitIndex = 0;     // Theo dõi vị trí bit trong byte hiện tại (0-7)
    QByteArray bitByte(1, 0x00); // Byte tạm cho group bit, khởi tạo 0

    for (int i = 0; i < dataStructure.size(); ++i)
    {
        const DataCsv &data = dataStructure[i];
        QString originalValue = m_userInputs.value(data.name, data.value); // Giữ nguyên dấu nháy kép

        if (data.type == "bit" && data.bitPosition >= 0 && data.bitPosition < 8)
        {
            // Xử lý bit trong group
            QString value = originalValue.remove("\""); // Loại bỏ nháy kép để kiểm tra "on"
            if (currentBitIndex < 8)
            {
                if (value.toLower() == "on")
                {
                    bitByte[0] |= (1 << data.bitPosition); // Đặt bit theo vị trí
                }
                currentBitIndex++;
            }

            // Kiểm tra nếu đã xử lý đủ 8 bit hoặc hết group
            if (currentBitIndex == 8 || (i + 1 == dataStructure.size()) || dataStructure[i + 1].type != "bit")
            {
                if (currentBitIndex > 0)
                {
                    if (!firstField)
                        body.append(delimiter);
                    body.append(bitByte); // Append ngay sau khi hoàn thành byte
                    firstField = false;
                    bitByte = QByteArray(1, 0x00); // Reset byte
                    currentBitIndex = 0;           // Reset vị trí bit
                }
            }
        }
        else
        {
            // Xử lý trường free, áp dụng padding đặc biệt cho CI500
            if (currentBitIndex > 0)
            {
                if (!firstField)
                    body.append(delimiter);
                body.append(bitByte); // Flush byte bit còn lại
                firstField = false;
                bitByte = QByteArray(1, 0x00);
                currentBitIndex = 0;
            }

            QByteArray valueData;
            if (originalValue.startsWith("\"") && originalValue.endsWith("\"") && deviceType == "CI500")
            {
                // Xử lý padding đặc biệt cho CI500
                QString contentWithSpaces = originalValue.mid(1, originalValue.length() - 2);
                QString trimmedContent = contentWithSpaces.trimmed();

                if (trimmedContent.isEmpty() || trimmedContent == "-" || trimmedContent == "---" || trimmedContent == "-.−")
                {
                    valueData = QByteArray(data.byteLimit, invalidPadding.at(0));
                }
                else
                {
                    int leftSpaces = 0;
                    int rightSpaces = 0;

                    for (int j = 0; j < contentWithSpaces.length(); j++)
                    {
                        if (contentWithSpaces[j] == ' ')
                            leftSpaces++;
                        else
                            break;
                    }
                    for (int j = contentWithSpaces.length() - 1; j >= 0; j--)
                    {
                        if (contentWithSpaces[j] == ' ')
                            rightSpaces++;
                        else
                            break;
                    }

                    QString actualData = contentWithSpaces.mid(leftSpaces, contentWithSpaces.length() - leftSpaces - rightSpaces);
                    QByteArray actualDataBytes = actualData.toUtf8();

                    valueData = QByteArray(leftSpaces, paddingByte.at(0)) +
                                actualDataBytes +
                                QByteArray(rightSpaces, paddingByte.at(0));

                    if (valueData.size() < data.byteLimit)
                    {
                        valueData.append(QByteArray(data.byteLimit - valueData.size(), paddingByte.at(0)));
                    }
                    else if (valueData.size() > data.byteLimit)
                    {
                        valueData = valueData.left(data.byteLimit);
                    }
                }
            }
            else
            {
                // Xử lý thông thường cho các device khác
                QString value = originalValue.remove("\"");
                if (value.isEmpty() || value == "-" || value == "---" || value == "-.−")
                {
                    valueData = QByteArray(data.byteLimit, invalidPadding.at(0));
                }
                else
                {
                    QByteArray rawData = value.toUtf8();
                    if (rawData.size() <= data.byteLimit)
                    {
                        int paddingSize = data.byteLimit - rawData.size();
                        valueData = rawData + QByteArray(paddingSize, paddingByte.at(0));
                    }
                    else
                    {
                        valueData = rawData.left(data.byteLimit);
                    }
                }
            }

            if (!firstField)
                body.append(delimiter);
            body.append(valueData);
            firstField = false;
        }
    }

    // Đảm bảo flush byte bit cuối cùng nếu còn
    if (currentBitIndex > 0)
    {
        if (!firstField)
            body.append(delimiter);
        body.append(bitByte);
    }

    frame.append(body);

    // 4. Thêm checksum nếu có
    if (frameConfig.contains("checksumType"))
    {
        QString checksumType = frameConfig.value("checksumType").toString();
        QByteArray dataForChecksum = body;
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
            QString checksumStr = QString::number(checksum % 256, 16).rightJustified(2, '0');
            frame.append(checksumStr.toUtf8());
        }
    }

    // 5. Thêm endByte
    if (frameConfig.contains("endByte"))
    {
        frame.append(QByteArray::fromHex(frameConfig.value("endByte").toString().remove("0x").toUtf8()));
    }

    // 6. In frame dưới dạng dễ đọc
    QString readableFrame;
    int delimiterPos = frame.indexOf(delimiter);
    if (delimiterPos != -1)
    {
        QByteArray header = frame.left(delimiterPos);
        readableFrame = QString(header).trimmed();

        QByteArray data = frame.mid(delimiterPos + delimiter.size());
        char delimiterChar = delimiter.isEmpty() ? '\0' : delimiter.at(0);
        QList<QByteArray> fields = data.split(delimiterChar);

        for (int i = 0; i < fields.size(); ++i)
        {
            QString field = QString(fields[i]).trimmed();
            if (!field.isEmpty())
            {
                if (i > 0)
                    readableFrame += ", ";
                readableFrame += field;
                if (i % 2 == 1 || field.length() > 10)
                    readableFrame += "\n";
            }
        }
    }
    else
    {
        readableFrame = QString(frame).trimmed();
    }

    qDebug().noquote() << "Built frame for device" << deviceType << "(readable):" << readableFrame;
    qDebug() << "Built frame for device" << deviceType << "(hex):" << frame.toHex();

    return frame;
}