// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h" // 包含由 .ui 文件生成的头文件
#include "gamestate.h"

MainWindow::MainWindow(GameState* state,QWidget *parent)
    : m_state(state),QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // 这行代码会根据 .ui 文件创建所有 UI 元素，包括你的 LogViewerWidget

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout *gameMainLayout = new QGridLayout(centralWidget);

    //日志
    m_logViewer=new LogViewerWidget(centralWidget);
    QObject::connect(m_state, &GameState::logMessageAdded,this, &LogViewerWidget::appendLogMessage);
    gameMainLayout->addWidget(m_logViewer,0,13,9,3);


}

MainWindow::~MainWindow()
{
    delete ui;
}
