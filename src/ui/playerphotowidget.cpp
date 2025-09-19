#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication>
#include <QLabel>
#include <QTimer>
#include <QStyle> // For style()->polish()
#include <QGraphicsView> // For viewport()

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    : QWidget(parent)
    , m_player(player)
    , m_mainLayout(new QVBoxLayout(this))
    , m_graphicsScene(new QGraphicsScene(this))
    , m_graphicsView(new QGraphicsView(m_graphicsScene, this))
    , m_videoItem(new QGraphicsVideoItem()) // <-- 标准 QGraphicsVideoItem
    , m_mediaPlayer(new QMediaPlayer(this))
    , m_textContainerWidget(new QWidget(this))
    , m_textLayout(new QGridLayout(m_textContainerWidget))
    , m_nameLabel(new QLabel(m_textContainerWidget))
    , m_coinsLabel(new CoinsWidget(m_textContainerWidget))
    , m_textProxy(nullptr)
{
    // PlayerPhotoWidget 作为一个纯粹的布局容器，不进行自定义绘制
    this->setContentsMargins(0, 0, 0, 0);
    this->setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground, true); // 确保PlayerPhotoWidget自身透明

    // 1. 设置QMediaPlayer和视频路径
    m_mediaPlayer->setVideoOutput(m_videoItem);
    m_mediaPlayer->setLoops(QMediaPlayer::Infinite);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media Player Error:" << error << errorString;
    });

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
    m_videoItem->setZValue(0);

    // 3. 设置文本标签并将其添加到m_textContainerWidget的QGridLayout中
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_nameLabel->setText(player->getName());
    m_coinsLabel->setCoinsNum(player->getCoins());
    m_textLayout->addWidget(m_nameLabel, 4, 0, 1, 5);
    m_textLayout->addWidget(m_coinsLabel, 3, 3, 2, 2);
    for(int i = 0; i < 5; ++i) {
        m_textLayout->setRowStretch(i, 1);
        m_textLayout->setColumnStretch(i, 1);
    }
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(0);

    // 4. 将m_textContainerWidget包装成QGraphicsProxyWidget并添加到场景
    m_textProxy = m_graphicsScene->addWidget(m_textContainerWidget);
    m_textProxy->setZValue(10);

    // 5. 设置QGraphicsView的属性
    m_graphicsView->setRenderHint(QPainter::Antialiasing);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setContentsMargins(0, 0, 0, 0); // 移除视图的边距

    // ****** 关键修改：QGraphicsView 的样式表负责所有视觉效果 ******
    m_graphicsView->setStyleSheet(
        "QGraphicsView {"
        "   background-color: black;" /* 填充边框内部的圆角区域和视频未覆盖的区域 */
        "   border: 2px solid #FFD700;" /* 金色边框 */
        "   border-radius: 15px;"       /* 圆角半径，同时裁剪视口和边框 */
        "}"
        "QGraphicsView[isCurrent=\"true\"] {" /* 当isCurrent属性为true时，边框变粗 */
        "   border-width: 6px;"
        "   border-color: #FFD700;" /* 保持金色 */
        "}"
        );
    // 初始化isCurrent属性为false，确保默认边框生效
    m_graphicsView->setProperty("isCurrent", false);
    m_graphicsView->style()->polish(m_graphicsView); // 立即应用样式表

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_graphicsView);
    setLayout(m_mainLayout);

    QTimer::singleShot(0, this, &PlayerPhotoWidget::adjustItemPositions);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){
    // 清理由 Qt 父子机制处理
}

void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    adjustItemPositions();
}

void PlayerPhotoWidget::adjustItemPositions() {
    // 获取QGraphicsView的视口矩形，这是实际绘制内容的区域，不包含边框
    const QRectF viewportRect = m_graphicsView->viewport()->rect();
    int width = viewportRect.width();
    int height = viewportRect.height();

    if (width <= 0 || height <= 0) { // 使用 <= 0 确保尺寸有效
        return;
    }

    // 场景的矩形应与视口矩形精确匹配
    m_graphicsScene->setSceneRect(viewportRect);

    // 视频项大小应填充整个场景（即QGraphicsView的视口）
    m_videoItem->setSize(QSizeF(width, height));
    m_videoItem->setPos(0, 0);

    // 确保视频内容完全覆盖 QGraphicsView 的整个视口
    // Qt::KeepAspectRatioByExpanding 会放大视频以填充整个 *矩形* 区域，
    // 然后 QGraphicsView 的 border-radius 会将其裁剪为圆角。
    m_graphicsView->fitInView(m_graphicsScene->sceneRect(), Qt::KeepAspectRatioByExpanding);

    int fontSize = qMax(5, height / 12); // 字体大小基于viewport高度
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

    if (m_textProxy) {
        // 文本代理也应覆盖整个场景（即QGraphicsView的视口）
        m_textProxy->setPos(0, 0);
        m_textProxy->resize(width, height);
        m_textContainerWidget->resize(width, height);
        m_textContainerWidget->updateGeometry();
        m_textContainerWidget->repaint();
    }
}

void PlayerPhotoWidget::onCoinsChange(Player *player, int coins){
    if(player!=m_player)
        return;
    m_coinsLabel->setCoinsNum(player->getCoins());
    if (m_textProxy) {
        m_textProxy->update();
    }
}

void PlayerPhotoWidget::onCurrentPlayerChanged(Player* currentPlayer) {
    if (m_player == currentPlayer) {
        m_mediaPlayer->play();
        m_graphicsView->setProperty("isCurrent", true); // 将属性设置到 QGraphicsView
    } else {
        m_mediaPlayer->pause();
        m_graphicsView->setProperty("isCurrent", false); // 将属性设置到 QGraphicsView
    }
    // 强制 QGraphicsView 重新评估其样式表并重绘
    m_graphicsView->style()->polish(m_graphicsView);
    m_graphicsView->update(); // 更新 QGraphicsView
    // 重新调整内部布局，因为边框宽度可能改变了有效内容区
    // 并且 QGraphicsView 的 viewport() 尺寸可能因此改变
    adjustItemPositions();
}
