// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h" // 包含由 .ui 文件生成的头文件

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // 这行代码会根据 .ui 文件创建所有 UI 元素，包括你的 LogViewerWidget

}

MainWindow::~MainWindow()
{
    delete ui;
}

LogViewerWidget* MainWindow::getLogViewerWidget() const {
    return ui->logWidget;
}

CardStoreAreaWidget* MainWindow::getCardStoreAreaWidget()const{
    return ui->cardStoreAreaWidget;
}
