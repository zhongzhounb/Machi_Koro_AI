// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LogViewerWidget;
class GameController;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    LogViewerWidget* getLogViewerWidget() const;

private:
    Ui::MainWindow *ui;
    GameController* m_game;
    // 你不需要在这里声明 LogViewerWidget* m_logViewer;
    // 因为 ui->setupUi(this) 会为你创建它，并通过 ui->logViewerWidgetName 访问
};
#endif // MAINWINDOW_H
