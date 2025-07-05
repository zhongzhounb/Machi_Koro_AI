#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug> // 用于调试输出
#include<QTimer>
#include "mainwindow.h"
#include "gamestate.h"      // 包含你的 GameState 头文件
#include "gamecontroller.h" // 包含你的 GameController 头文件

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 国际化设置
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Machi_Koro_AI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // 2. 创建游戏状态 (Model)
    // 让 QApplication 作为父对象，确保其生命周期与应用程序一致
    GameState* gameState = new GameState(&a);

    // 3. 创建主窗口 (View)
    MainWindow* w = new MainWindow();

    // 4. 创建游戏控制器 (Controller)
    GameController* gameController = new GameController(w,gameState,&a);

    // 5. （给界面布局时间）调用控制器的方法来初始化游戏逻辑
    QTimer::singleShot(0, [gameController]() {
         gameController->initializeGame();
    });

    w->show(); // 显示主窗口

    return a.exec(); // 启动 Qt 事件循环
}
