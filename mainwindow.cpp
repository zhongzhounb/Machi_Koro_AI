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
    setContentsMargins(0, 0, 0, 0);


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
    for(int i = 0; i < 90; ++i) { // 90 行 (索引 0 到 89)
        gameMainLayout->setRowStretch(i, 1);
    }
    for(int i = 0; i < 160; ++i) { // 160 列 (索引 0 到 159)
        gameMainLayout->setColumnStretch(i, 1);
    }

    // 根据图片放置各个彩色块
    // 注意：Excel 的 A1 对应 Qt 的 (0,0)
    // 所有坐标和跨度都已乘以10

    QList<Player*> players=state->getPlayers();
    // 当前玩家
    QWidget* playerPhoto0 = new QWidget(gameMain);
    playerPhoto0->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(playerPhoto0, 70, 0, 20, 20);

    PlayerAreaWidget* playerCardArea0=new PlayerAreaWidget(players[0],true,false,this);
    QObject::connect(players[0], &Player::cardAdded, playerCardArea0, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerCardArea0, 75, 20, 15, 120);

    PlayerAreaWidget* playerLandmarkArea0=new PlayerAreaWidget(players[0],true,true,this);
    QObject::connect(players[0], &Player::cardAdded, playerLandmarkArea0, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerLandmarkArea0, 65, 45, 10, 70);


    // 1号玩家
    QWidget* playerPhoto1 = new QWidget(gameMain);
    playerPhoto1->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(playerPhoto1, 0, 0, 15, 15);

    PlayerAreaWidget* playerCardArea1=new PlayerAreaWidget(players[1],false,false,this);
    QObject::connect(players[1], &Player::cardAdded, playerCardArea1, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerCardArea1, 15, 0, 50, 12);

    PlayerAreaWidget* playerLandmarkArea1=new PlayerAreaWidget(players[1],false,true,this);
    QObject::connect(players[1], &Player::cardAdded, playerLandmarkArea1, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerLandmarkArea1, 15, 12, 50, 8);

    // 2号玩家
    QWidget* playerPhoto2 = new QWidget(gameMain);
    playerPhoto2->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(playerPhoto2, 0, 70, 15, 15);

    PlayerAreaWidget* playerCardArea2=new PlayerAreaWidget(players[2],true,false,this);
    QObject::connect(players[2], &Player::cardAdded, playerCardArea2, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerCardArea2, 0, 20, 12, 50);

    PlayerAreaWidget* playerLandmarkArea2=new PlayerAreaWidget(players[2],true,true,this);
    QObject::connect(players[2], &Player::cardAdded, playerLandmarkArea2, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerLandmarkArea2, 12, 20, 8, 50);

    // 3号玩家
    QWidget* playerPhoto3 = new QWidget(gameMain);
    playerPhoto3->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(playerPhoto3, 0, 140, 15, 15);
    PlayerAreaWidget* playerCardArea3=new PlayerAreaWidget(players[3],true,false,this);
    QObject::connect(players[3], &Player::cardAdded, playerCardArea3, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerCardArea3, 0, 90, 12, 50);

    PlayerAreaWidget* playerLandmarkArea3=new PlayerAreaWidget(players[3],true,true,this);
    QObject::connect(players[3], &Player::cardAdded, playerLandmarkArea3, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerLandmarkArea3, 12, 90, 8, 50);


    // 4号玩家
    QWidget* playerPhoto4 = new QWidget(gameMain);
    playerPhoto4->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(playerPhoto4, 70, 145, 15, 15);
    PlayerAreaWidget* playerCardArea4=new PlayerAreaWidget(players[4],false,false,this);
    QObject::connect(players[4], &Player::cardAdded, playerCardArea4, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerCardArea4, 20, 148, 50, 12);

    PlayerAreaWidget* playerLandmarkArea4=new PlayerAreaWidget(players[4],false,true,this);
    QObject::connect(players[4], &Player::cardAdded, playerLandmarkArea4, &PlayerAreaWidget::onCardAdded);
    gameMainLayout->addWidget(playerLandmarkArea4, 20, 140, 50, 8);


    // 商店
    m_cardStoreArea = new CardStoreAreaWidget(gameMain); // 如果 CardStoreAreaWidget 还没有创建
    m_cardStoreArea->setGameState(m_state);
    gameMainLayout->addWidget(m_cardStoreArea, 23, 45, 40, 70);



    // 3. 蓝色块 (右侧垂直: N1-P9) - 对应您的 m_logViewer
    // 对应原始行 0-8 (9行), 列 13-15 (3列) -> 0行, 130列, 跨90行, 跨30列
    /*QWidget* logViewerDummy = new QWidget(gameMain); // 仅为演示，实际使用 m_logViewer
    logViewerDummy->setStyleSheet("background-color: blue;");
    gameMainLayout->addWidget(logViewerDummy, 0, 130, 90, 30);*/

    // 将 gameMain 添加到 centralLayout 中，它会填充 centralWidget
    centralLayout->addWidget(gameMain);
}

MainWindow::~MainWindow()
{
    delete ui;
}
