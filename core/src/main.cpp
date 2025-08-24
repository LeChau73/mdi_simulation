#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MainWindow.h"
#include "ConfigManager.h"
#include "Log.h"
#include <QLoggingCategory>

// Tắt tất cả các log từ module qt.qml
// QLoggingCategory::setFilterRules("qt.qml.*=false");

int main(int argc, char *argv[])
{
    // Tắt các cảnh báo về binding trong QML
    QLoggingCategory::setFilterRules("qt.qml.binding.warning=false");
    QLoggingCategory::setFilterRules("qt.qml.*=false");
    QGuiApplication app(argc, argv);

    // Lấy đường dẫn thư mục chứa file thực thi
    QString exeDir = QCoreApplication::applicationDirPath();
    qDebug() << "Thư mục thực thi:" << exeDir;

    // Lấy đường dẫn đầy đủ của file thực thi
    QString exePath = QCoreApplication::applicationFilePath();
    LOG_INFO() << "Đường dẫn file thực thi:" << exePath;

    // Khởi tạo engine QML
    QQmlApplicationEngine engine;

    ConfigManager *config = new ConfigManager(&app);

    config->loadFromFile(exeDir + "/default.config"); // Lệnh gọi hàm này cũng chưa hoàn chỉnh.

    // Tạo instance duy nhất của MainWindow

    MainWindow mainWindow;
    LOG_INFO() << config->m_device << config->command;
    mainWindow.loadDevice("PB", "MISCF", exeDir);
    // Đăng ký MainWindow với QML dưới tên "mainWindowController"

    engine.rootContext()->setContextProperty("mainWindowController", &mainWindow);
    qDebug() << "Context property 'mainWindowController' set to:" << &mainWindow;

    // Tải file QML chính (giả định là main.qml trong resource hoặc filesystem)
    const QUrl url(QStringLiteral("qrc:/main.qml")); // Thay bằng đường dẫn thực tế nếu không dùng resource
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
                     {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    qDebug() << "Loading QML from:" << url;
    engine.load(url);

    // Chạy ứng dụng
    // Enters the main event loop and waits until exit() is called
    return app.exec();
}