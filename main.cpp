#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

void registerAllCommands();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Machi_Koro_AI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // 在应用程序启动时注册所有命令和卡牌
    registerAllCommands();

    MainWindow w;
    w.show();
    return a.exec();
}
