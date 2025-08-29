// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "global.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LogViewerWidget;
class CardStoreAreaWidget;
class GameState;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(GameState* state,QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onRequestUserInput(PromptData promptData);

signals:
    void responseUserInput(int optionId);

private:
    GameState* m_state;
    Ui::MainWindow *ui;
    LogViewerWidget* m_logViewer;
    CardStoreAreaWidget* m_cardStoreArea;
    // 因为 ui->setupUi(this) 会为你创建它，并通过 ui->logViewerWidgetName 访问
};
#endif // MAINWINDOW_H
