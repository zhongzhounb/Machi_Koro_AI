// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h" // 包含由 .ui 文件生成的头文件
#include "gamestate.h"      // 假设这个头文件仍然需要
#include "QStackedLayout"
#include "QGridLayout"
#include "QWidget"          // 用于创建通用控件来表示彩色块
#include "cardstoreareawidget.h"
#include "playerareawidget.h"
#include "player.h"
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(GameState* state, QWidget *parent)
    : m_state(state), QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // 设置主界面
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 分层布局 (QStackedLayout)
    QStackedLayout* centralLayout = new QStackedLayout(centralWidget);
    // 全显示模式，gameMain 将填充 centralWidget
    centralLayout->setStackingMode(QStackedLayout::StackAll);

    // 游戏主布局层
    QWidget* gameMain = new QWidget(centralWidget); // gameMain 是 centralWidget 的子控件
    QGridLayout *gameMainLayout = new QGridLayout(gameMain); // gameMainLayout 应用于 gameMain

    // 设置网格的边距和间距为0，以实现紧密的网格布局（如图片所示）
    gameMainLayout->setContentsMargins(0, 0, 0, 0);
    gameMainLayout->setSpacing(0);

    // 设置所有行和列的伸缩因子为1，确保所有单元格大小相等
    for(int i = 0; i < 9; ++i) { // 9 行 (索引 0 到 8)
        gameMainLayout->setRowStretch(i, 1);
    }
    for(int i = 0; i < 16; ++i) { // 16 列 (索引 0 到 15)
        gameMainLayout->setColumnStretch(i, 1);
    }

    // 根据图片放置各个彩色块
    // 注意：Excel 的 A1 对应 Qt 的 (0,0)

    QList<Player*> players=state->getPlayers();

    // 1. 红色块 (左上角: A1-F2)
    // 对应行 0-1 (2行), 列 0-5 (6列)
    QWidget* redTopBlock = new QWidget(gameMain);
    redTopBlock->setStyleSheet("background-color: red;");
    gameMainLayout->addWidget(redTopBlock, 0, 0, 2, 6);

    // 2. 绿色块 (中上部: H1-M2)
    // 对应行 0-1 (2行), 列 7-12 (6列)
    QWidget* greenTopBlock = new QWidget(gameMain);
    greenTopBlock->setStyleSheet("background-color: lightgreen;");
    gameMainLayout->addWidget(greenTopBlock, 0, 7, 2, 6);

    // 3. 蓝色块 (右侧垂直: N1-P9) - 对应您的 m_logViewer
    // 对应行 0-8 (9行), 列 13-15 (3列)
    // 假设 m_logViewer 已经创建并连接了信号
    // m_logViewer = new LogViewerWidget(gameMain); // 如果 LogViewerWidget 还没有创建
    // QObject::connect(m_state, &GameState::logMessageAdded, m_logViewer, &LogViewerWidget::appendLogMessage);
    QWidget* logViewerDummy = new QWidget(gameMain); // 仅为演示，实际使用 m_logViewer
    logViewerDummy->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(logViewerDummy, 0, 13, 9, 3); // 0行, 13列, 跨9行, 跨3列

    // 4. 红色块 (左侧垂直: A3-B8)
    // 对应行 2-7 (6行), 列 0-1 (2列)
    QWidget* redVerticalBlock = new QWidget(gameMain);
    redVerticalBlock->setStyleSheet("background-color: darkred;");
    gameMainLayout->addWidget(redVerticalBlock, 2, 0, 5, 2);

    // 5. 紫色块 (中间大块: D3-K6) - 对应您的 m_cardStoreArea
    // 对应行 2-5 (4行), 列 3-10 (8列)
    // 注意：您之前的代码是 4 行 7 列，根据图片应该是 4 行 8 列 (D到K)
    // 假设 m_cardStoreArea 已经创建并设置了 GameState
    m_cardStoreArea = new CardStoreAreaWidget(gameMain); // 如果 CardStoreAreaWidget 还没有创建
    m_cardStoreArea->setGameState(m_state);
    //QWidget* cardStoreAreaDummy = new QWidget(gameMain); // 仅为演示，实际使用 m_cardStoreArea
    //cardStoreAreaDummy->setStyleSheet("background-color: purple;");
    gameMainLayout->addWidget(m_cardStoreArea, 2, 3, 4, 7); // 2行, 3列, 跨4行, 跨8列 (已修正为8列)

    // 6. 绿色块 (右侧中间垂直: L3-M8)
    // 对应行 2-7 (6行), 列 11-12 (2列)
    QWidget* greenMiddleBlock = new QWidget(gameMain);
    greenMiddleBlock->setStyleSheet("background-color: seagreen;");
    gameMainLayout->addWidget(greenMiddleBlock, 2, 11, 5, 2);

    // 7. 蓝色块 (底中部: I7-K7)
    // 对应行 6 (1行), 列 8-10 (3列)
    QWidget* blueBottomBlock = new QWidget(gameMain);
    blueBottomBlock->setStyleSheet("background-color: dodgerblue;");
    gameMainLayout->addWidget(blueBottomBlock, 6, 8, 1, 3);

    // 8. 黄色块 (底部大块: A8-M9)
    // 对应行 7-8 (2行), 列 0-12 (13列)
    PlayerAreaWidget* player1CardArea=new PlayerAreaWidget(players[0],true,false,this);
    QObject::connect(players[0], &Player::cardAdded, player1CardArea, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(player1CardArea, 7, 0, 2, 13);

    // 将 gameMain 添加到 centralLayout 中，它会填充 centralWidget
    centralLayout->addWidget(gameMain);
}

MainWindow::~MainWindow()
{
    delete ui;
}
