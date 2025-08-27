bool firstField = true;
QByteArray body;

// Biến phục vụ cho bit field
QByteArray currentBitByte;  
bool inBitGroup = false;    
QString currentGroup;

for (int i = 0; i < dataStructureTemp.size(); i++)
{
    DataCsv &data = dataStructureTemp[i];

    if (data.send == 1)
    {
        // Nếu là BIT FIELD
        if (data.type == "bit" && !data.group.isEmpty())
        {
            // Nếu bắt đầu 1 group mới
            if (!inBitGroup) {
                currentBitByte = QByteArray(1, 0x00);
                inBitGroup = true;
                currentGroup = data.group;
            }

            // Set bit theo bitPosition
            QString value = data.value.remove("\"");
            if (data.bitPosition >= 0 && data.bitPosition < 8) {
                if (value.toLower() == "on" || value == "1") {
                    currentBitByte[0] |= (1 << data.bitPosition);
                }
            }

            // Nếu đây là bit cuối trong group → flush ra body
            bool lastInGroup = (i+1 >= dataStructureTemp.size()) 
                               || (dataStructureTemp[i+1].group != currentGroup);

            if (lastInGroup) {
                if (!firstField) body.append(delimiter);
                body.append(currentBitByte);
                firstField = false;
                inBitGroup = false;
            }

            continue; // Xong xử lý bit → sang field tiếp theo
        }

        // ===== Xử lý FIELD THƯỜNG =====
        QString originalValue = data.value; 
        QByteArray valueData;

        if (originalValue.startsWith("\"") && originalValue.endsWith("\""))
        {
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

                for (int j = 0; j < contentWithSpaces.length(); j++) {
                    if (contentWithSpaces[j] == ' ') leftSpaces++;
                    else break;
                }
                for (int j = contentWithSpaces.length() - 1; j >= 0; j--) {
                    if (contentWithSpaces[j] == ' ') rightSpaces++;
                    else break;
                }

                QString actualData = contentWithSpaces.mid(leftSpaces, contentWithSpaces.length() - leftSpaces - rightSpaces);
                QByteArray actualDataBytes = actualData.toUtf8();

                valueData = QByteArray(leftSpaces, paddingByte.at(0)) +
                            actualDataBytes +
                            QByteArray(rightSpaces, paddingByte.at(0));

                if (valueData.size() < data.byteLimit) {
                    valueData.append(QByteArray(data.byteLimit - valueData.size(), paddingByte.at(0)));
                } else if (valueData.size() > data.byteLimit) {
                    valueData = valueData.left(data.byteLimit);
                }
            }
        }
        else
        {
            QString value = originalValue;
            if (value.isEmpty() || value == "-" || value == "---" || value == "-.−")
            {
                valueData = QByteArray(data.byteLimit, invalidPadding.at(0));
            }
            else
            {
                QByteArray rawData = value.toUtf8();
                if (rawData.size() <= data.byteLimit) {
                    valueData = rawData + QByteArray(data.byteLimit - rawData.size(), paddingByte.at(0));
                } else {
                    valueData = rawData.left(data.byteLimit);
                }
            }
        }

        if (!firstField) {
            body.append(delimiter);
        }
        body.append(valueData);
        firstField = false;
    }
    else
    {
        // Field không gửi → full padding
        QByteArray paddingData = QByteArray(data.byteLimit, paddingByte.at(0));
        if (!firstField) {
            body.append(delimiter);
        }
        body.append(paddingData);
        firstField = false;
    }
}
