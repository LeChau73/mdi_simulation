#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../include/MainWindow.h"
#include "../include/FileDataLoader.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Khởi tạo engine QML
    QQmlApplicationEngine engine;

    // Tạo instance duy nhất của MainWindow
    MainWindow mainWindow;

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
    return app.exec();
}