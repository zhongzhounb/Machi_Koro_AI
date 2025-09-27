#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication>
#include <QLabel>
#include <QTimer>
#include <QStyle>
#include <QPainter>
#include <QPainterPath>
#include "roundedvideoitem.h"

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    : QWidget(parent)
    , m_player(player)
    , m_mainLayout(new QVBoxLayout(this))
    , m_graphicsScene(new QGraphicsScene(this))
    , m_graphicsView(new QGraphicsView(m_graphicsScene, this))
    , m_videoItem(new RoundedVideoItem())
    , m_mediaPlayer(new QMediaPlayer(this))
    , m_textContainerWidget(new QWidget(this))
    , m_textLayout(new QGridLayout(m_textContainerWidget))
    , m_nameLabel(new QLabel(m_textContainerWidget))
    , m_coinsLabel(new CoinsWidget(m_textContainerWidget))
    , m_textProxy(nullptr)
    , m_currentBorderWidth(2) // 初始化默认细边框宽度
    , m_borderRadius(15.0)    // 初始化圆角半径
{
    // 确保PlayerPhotoWidget可以接收paintEvent
    this->setContentsMargins(0, 0, 0, 0); // 移除自身的边距
    // 启用背景绘制，以便paintEvent可以绘制
    this->setAutoFillBackground(false); // 不让Qt自动填充背景，我们自己画

    // 1. 设置QMediaPlayer和QGraphicsVideoItem
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

    // QGraphicsView的样式表只负责透明背景和圆角裁剪，不绘制边框
    m_graphicsView->setStyleSheet(
        QString("QGraphicsView {"
                "   background-color: transparent;" // 确保QGraphicsView自身背景透明
                "   border: none;" // QGraphicsView自身不绘制边框
                "   border-radius: %1px;" // 裁剪视图内容为圆角
                "}")
            .arg(m_borderRadius)
        );

    // 主布局的边距应该根据边框宽度来设置，确保QGraphicsView位于边框内部
    m_mainLayout->setContentsMargins(m_currentBorderWidth, m_currentBorderWidth, m_currentBorderWidth, m_currentBorderWidth);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_graphicsView);
    setLayout(m_mainLayout);

    // 延迟调用 adjustItemPositions()，确保widget已经有了正确的初始大小
    QTimer::singleShot(0, this, &PlayerPhotoWidget::adjustItemPositions);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){
    // Cleanup handled by Qt parent-child mechanism
}

void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    adjustItemPositions();
    update(); // 请求重绘PlayerPhotoWidget，以更新边框和背景
}

void PlayerPhotoWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 绘制背景（白色圆角）
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(rect(), m_borderRadius, m_borderRadius);
    painter.fillPath(backgroundPath, Qt::white);

    // 2. 绘制边框（金色圆角）
    QPen pen(QColor("#FFD700")); // 金色
    pen.setWidth(m_currentBorderWidth); // 使用当前边框宽度
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush); // 不填充路径，只画轮廓

    painter.drawPath(backgroundPath); // 在背景路径上绘制边框
}


void PlayerPhotoWidget::adjustItemPositions() {
    const QSize viewSize = m_graphicsView->size();
    int width = viewSize.width();
    int height = viewSize.height();

    if (width == 0 || height == 0) {
        return;
    }

    m_graphicsScene->setSceneRect(0, 0, width, height);

    m_videoItem->setSize(QSizeF(width, height));
    m_videoItem->setPos(0, 0);

    m_videoItem->setBorderRadius(m_borderRadius);

    m_graphicsView->fitInView(m_graphicsScene->sceneRect(), Qt::KeepAspectRatioByExpanding);

    int fontSize = qMax(5, height / 12);
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

    if (m_textProxy) {
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
        m_currentBorderWidth = 16; // 粗边框
    } else {
        m_mediaPlayer->pause();
        m_currentBorderWidth = 8; // 细边框
    }
    // 每次边框宽度改变时，更新主布局的边距，并请求重绘PlayerPhotoWidget
    m_mainLayout->setContentsMargins(m_currentBorderWidth, m_currentBorderWidth, m_currentBorderWidth, m_currentBorderWidth);
    update(); // 请求重绘PlayerPhotoWidget，以更新边框和背景
}
