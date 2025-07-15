#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication> // 用于获取应用程序路径
#include <QMediaMetaData> // 用于获取视频元数据，尽管在这个修改中我们不直接使用它来调整widget大小，但了解其用法很重要

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    :QWidget(parent)
    ,m_player(player)
    ,m_mainLayout(new QStackedLayout(this))
    ,m_videoContainer(new QWidget(this))
    //,m_videoLayout(new QGridLayout(this))
    ,m_videoWidget(new QVideoWidget(this))
    ,m_mediaPlayer(new QMediaPlayer(this))
    ,m_textContainer(new QWidget(this))
    //,m_textLayout(new QGridLayout(this))
    ,m_nameLabel(new QLabel(this))
    ,m_coinsLabel(new CoinsWidget(this))
{

    m_mainLayout->setStackingMode(QStackedLayout::StackAll);

    // 将 QVideoWidget 设置为 QMediaPlayer 的视频输出
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    // 设置视频宽高比模式为“保持宽高比并扩展”，以填充整个区域，避免黑边（可能会裁剪视频边缘）
    m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    // 设置视频部件背景透明，以防万一出现边框
    m_videoWidget->setStyleSheet("background-color: transparent;");
    // 设置循环播放
    m_mediaPlayer->setLoops(QMediaPlayer::Infinite); // 或者使用 -1
    // 连接媒体播放器状态变化的信号
    //connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerPhotoWidget::onMediaStatusChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media Player Error:" << error << errorString;
    });

    // 设置视频文件路径
    QString videoResourcePath ; // <--- 修改为您的实际资源路径
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
    m_mediaPlayer->play();

    //当前文字居中
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setText(player->getName());

    //设置文字
    m_coinsLabel->setCoinsNum(player->getCoins());

    m_videoLayout=new QGridLayout(m_videoContainer);
    m_videoLayout->addWidget(m_videoWidget,0,0,4,4);

    m_textLayout=new QGridLayout(m_textContainer);
    m_textLayout->addWidget(m_nameLabel,4,0,1,5);
    m_textLayout->addWidget(m_coinsLabel,2,3,2,2);

    //设置为5*5大均分网格
    for(int i = 0; i < 5; ++i) {
        m_videoLayout->setRowStretch(i, 1);
        m_textLayout->setRowStretch(i, 1);
    }
    for(int i = 0; i < 5; ++i) {
        m_videoLayout->setColumnStretch(i, 1);
        m_videoLayout->setColumnStretch(i, 1);
    }

    m_mainLayout->addWidget(m_videoContainer);
    m_mainLayout->addWidget(m_textContainer);

    setLayout(m_mainLayout);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){}


void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    // 根据新的有效高度更新字体
    int fontSize = qMax(5, height / 12);
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
}

void PlayerPhotoWidget::onCoinsChange(Player *player, int coins){
    if(player!=m_player)
        return;
    m_coinsLabel->setCoinsNum(player->getCoins());
}

