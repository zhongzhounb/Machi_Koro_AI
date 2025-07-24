#include "playerphotowidget.h"
#include "player.h" // 确保包含Player的定义
#include "coinswidget.h" // 确保包含CoinsWidget的定义
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication>
#include <QLabel> // 确保包含QLabel的定义
#include <QTimer> // 新增：用于singleShot

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    : QWidget(parent)
    , m_player(player)
    , m_mainLayout(new QVBoxLayout(this)) // 使用QVBoxLayout作为主布局
    , m_graphicsScene(new QGraphicsScene(this)) // 初始化场景
    , m_graphicsView(new QGraphicsView(m_graphicsScene, this)) // 初始化视图，并关联场景
    , m_videoItem(new QGraphicsVideoItem()) // 初始化视频项
    , m_mediaPlayer(new QMediaPlayer(this))
    , m_textContainerWidget(new QWidget(this)) // 初始化文本容器
    , m_textLayout(new QGridLayout(m_textContainerWidget)) // 初始化文本容器的布局
    , m_nameLabel(new QLabel(m_textContainerWidget)) // 将QLabel的父对象设置为m_textContainerWidget
    , m_coinsLabel(new CoinsWidget(m_textContainerWidget)) // 将CoinsWidget的父对象设置为m_textContainerWidget
    , m_textProxy(nullptr) // 初始化为nullptr
{
    // 1. 设置QMediaPlayer和QGraphicsVideoItem
    m_mediaPlayer->setVideoOutput(m_videoItem); // 将视频输出设置给QGraphicsVideoItem
    m_mediaPlayer->setLoops(QMediaPlayer::Infinite); // 循环播放

    // 连接媒体播放器错误信号
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media Player Error:" << error << errorString;
    });

    // 设置视频文件路径
    QString videoResourcePath;
    if(m_player->getId()==1)
        videoResourcePath = "qrc:/resources/images/player/video/Amy/ordinary.mp4";
    else if(m_player->getId()==2)
        videoResourcePath ="qrc:/resources/images/player/video/Xiyue/ordinary.mp4";
    else if(m_player->getId()==3)
        videoResourcePath ="qrc:/resources/images/player/video/Lily/ordinary.mp4";
    else if(m_player->getId()==4)
        videoResourcePath ="qrc:/resources/images/player/video/Xingchen/ordinary.mp4";
    else if(m_player->getId()==5)
        videoResourcePath ="qrc:/resources/images/player/video/Liuli/ordinary.mp4";

    m_mediaPlayer->setSource(QUrl(videoResourcePath));
    m_mediaPlayer->pause();

    // 2. 将视频项添加到场景中，并设置Z值（视频在底层）
    m_graphicsScene->addItem(m_videoItem);
    m_videoItem->setZValue(0); // 视频在最底层

    // 3. 设置文本标签并将其添加到m_textContainerWidget的QGridLayout中
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_nameLabel->setText(player->getName());

    m_coinsLabel->setCoinsNum(player->getCoins());

    // 将标签添加到m_textLayout中，使用原始的网格布局参数
    m_textLayout->addWidget(m_nameLabel, 4, 0, 1, 5); // 名字标签在第4行，第0列，跨1行5列
    m_textLayout->addWidget(m_coinsLabel, 3, 3, 2, 2); // 金币标签在第3行，第3列，跨2行2列

    // 设置5x5均分网格的拉伸因子
    for(int i = 0; i < 5; ++i) {
        m_textLayout->setRowStretch(i, 1);
        m_textLayout->setColumnStretch(i, 1);
    }
    // 确保QGridLayout的边距为0，这样m_textContainerWidget可以紧密地包裹内容
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(0); // 移除网格单元格之间的间距

    // 4. 将m_textContainerWidget包装成QGraphicsProxyWidget并添加到场景
    m_textProxy = m_graphicsScene->addWidget(m_textContainerWidget);
    m_textProxy->setZValue(10); // 文本容器在视频之上

    // 5. 设置QGraphicsView的属性
    m_graphicsView->setRenderHint(QPainter::Antialiasing); // 启用抗锯齿
    m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // 确保整个视图更新
    m_graphicsView->setCacheMode(QGraphicsView::CacheBackground); // 缓存背景以提高性能
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 关闭滚动条
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 关闭滚动条
    m_graphicsView->setContentsMargins(0, 0, 0, 0); // 移除视图的边距
    m_mainLayout->setContentsMargins(0, 0, 0, 0); // 移除主布局的边距
    m_mainLayout->setSpacing(0); // 移除主布局的间距

    // 将QGraphicsView添加到主布局
    m_mainLayout->addWidget(m_graphicsView);
    setLayout(m_mainLayout);

    // 延迟调用 adjustItemPositions()，确保widget已经有了正确的初始大小
    QTimer::singleShot(0, this, &PlayerPhotoWidget::adjustItemPositions);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){
    // QGraphicsScene和QGraphicsView的析构函数会自动清理其管理的item
    // m_textContainerWidget作为m_textProxy的包装，也会被QGraphicsProxyWidget管理，无需手动delete
    // 其他成员变量作为PlayerPhotoWidget的直接成员，会自动析构
}

void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    // 每次窗口大小改变时，都会收到resizeEvent，此时可以安全地调整布局
    adjustItemPositions();
}

void PlayerPhotoWidget::adjustItemPositions() {
    const QSize allocatedSize = size(); // 获取当前widget的大小
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    // 如果widget的尺寸为0，则不进行布局，等待正确的尺寸到来
    if (width == 0 || height == 0) {
        return;
    }

    // 调整场景的Rect以匹配视图大小
    m_graphicsScene->setSceneRect(0, 0, width, height);
    // 调整视图以适应整个场景，并保持宽高比，确保视频填充整个区域
    m_graphicsView->fitInView(m_graphicsScene->sceneRect(), Qt::KeepAspectRatioByExpanding);

    // 调整视频项的大小以填充整个视图/场景
    m_videoItem->setSize(QSizeF(width, height));
    m_videoItem->setPos(0, 0); // 视频从场景的左上角开始

    // 根据新的有效高度更新字体
    int fontSize = qMax(5, height / 12); // 字体大小与高度成比例
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

    // 调整文本容器QGraphicsProxyWidget的位置和大小
    if (m_textProxy) {
        // 让m_textContainerWidget（通过m_textProxy）覆盖整个PlayerPhotoWidget区域
        m_textProxy->setPos(0, 0);
        m_textProxy->resize(width, height);

        // 关键修改：显式设置m_textContainerWidget的大小，使其内部布局能正确工作
        m_textContainerWidget->resize(width, height);

        // 强制m_textContainerWidget重新布局其内部的QGridLayout
        m_textContainerWidget->updateGeometry();
        m_textContainerWidget->repaint();
    }
}

void PlayerPhotoWidget::onCoinsChange(Player *player, int coins){
    if(player!=m_player)
        return;
    m_coinsLabel->setCoinsNum(player->getCoins());
    // 当CoinsWidget内容改变时，通知其父容器（m_textContainerWidget）更新，
    // 进而通过m_textProxy刷新场景中的显示
    if (m_textProxy) {
        m_textProxy->update(); // 强制代理小部件重绘
    }
}

void PlayerPhotoWidget::onCurrentPlayerChanged(Player* currentPlayer) {
    if (m_player == currentPlayer) {
        // 如果是当前玩家，则播放视频
        qDebug() << "Player" << m_player->getName() << "is current player. Playing video.";
        m_mediaPlayer->play();
    } else {
        // 如果不是当前玩家，则暂停视频
        qDebug() << "Player" << m_player->getName() << "is NOT current player. Pausing video.";
        m_mediaPlayer->pause();
    }
}
