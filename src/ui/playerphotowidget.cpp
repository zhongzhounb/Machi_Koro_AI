#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication>
#include <QPalette>
#include <QColor>
#include <QAudioOutput>
// #include "aspectratiowidget.h" // 已经在 .h 中包含

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    :QWidget(parent)
    ,m_player(player)
    ,m_mainLayout(new QGridLayout(this))
    ,m_nameLabel(new QLabel(this))
    ,m_videoWidget1(new QVideoWidget(this)) // 保持初始化 QVideoWidget
    ,m_videoWidget2(new QVideoWidget(this)) // 保持初始化 QVideoWidget
    // <--- 新增：初始化 AspectRatioWidget，传入 QVideoWidget 和期望的宽高比 (1:1)
    ,m_aspectRatioWidget1(new AspectRatioWidget(m_videoWidget1, 1.0f, 1.0f, this))
    ,m_aspectRatioWidget2(new AspectRatioWidget(m_videoWidget2, 1.0f, 1.0f, this))
    ,m_mediaPlayer1(new QMediaPlayer(this))
    ,m_mediaPlayer2(new QMediaPlayer(this))
    ,m_coinsLabel(new CoinsWidget(this))
    ,m_stackedLayout(new QStackedLayout())
    ,m_isPreloading(false)
{
    // 将视频部件设置为媒体播放器的视频输出
    m_mediaPlayer1->setVideoOutput(m_videoWidget1);
    m_mediaPlayer2->setVideoOutput(m_videoWidget2);

    // 设置两个视频部件的背景颜色为白色，以避免黑屏闪烁
    // 注意：AspectRatioWidget 的背景色也可能需要设置，或者确保其父部件背景色是白色
    m_videoWidget1->setStyleSheet("background-color: white;");
    m_videoWidget2->setStyleSheet("background-color: white;");
    // 确保 AspectRatioWidget 自身背景也是白色，这样当其内部的 QVideoWidget 缩小后，外部区域是白色
    m_aspectRatioWidget1->setStyleSheet("background-color: white;");
    m_aspectRatioWidget2->setStyleSheet("background-color: white;");


    // 将 AspectRatioWidget 添加到堆叠布局中
    m_stackedLayout->addWidget(m_aspectRatioWidget1); // <--- 修改：添加 AspectRatioWidget
    m_stackedLayout->addWidget(m_aspectRatioWidget2); // <--- 修改：添加 AspectRatioWidget

    // 连接两个媒体播放器的信号到统一的槽函数
    connect(m_mediaPlayer1, &QMediaPlayer::mediaStatusChanged, this, &PlayerPhotoWidget::onPlayerMediaStatusChanged);
    connect(m_mediaPlayer1, &QMediaPlayer::positionChanged, this, &PlayerPhotoWidget::onPlayerPositionChanged);
    connect(m_mediaPlayer1, &QMediaPlayer::errorOccurred, this, &PlayerPhotoWidget::onPlayerErrorOccurred);

    connect(m_mediaPlayer2, &QMediaPlayer::mediaStatusChanged, this, &PlayerPhotoWidget::onPlayerMediaStatusChanged);
    connect(m_mediaPlayer2, &QMediaPlayer::positionChanged, this, &PlayerPhotoWidget::onPlayerPositionChanged);
    connect(m_mediaPlayer2, &QMediaPlayer::errorOccurred, this, &PlayerPhotoWidget::onPlayerErrorOccurred);

    // 设置要循环播放的视频文件路径
    if(m_player->getId()==1)
        m_videoUrl = QUrl("qrc:/resources/images/player/video/Amy/ordinary.mp4");
    else if(m_player->getId()==2)
        m_videoUrl = QUrl("qrc:/resources/images/player/video/Xiyue/ordinary.mp4");
    else if(m_player->getId()==3)
        m_videoUrl = QUrl("qrc:/resources/images/player/video/Lily/ordinary.mp4");
    else if(m_player->getId()==4)
        m_videoUrl = QUrl("qrc:/resources/images/player/video/Xingchen/ordinary.mp4");
    else if(m_player->getId()==5)
        m_videoUrl = QUrl("qrc:/resources/images/player/video/Liuli/ordinary.mp4");

    // 为两个播放器设置相同的视频源
    m_mediaPlayer1->setSource(m_videoUrl);
    m_mediaPlayer2->setSource(m_videoUrl);

    // 初始化状态：m_mediaPlayer1 为活跃播放器，m_mediaPlayer2 为待机播放器
    m_activePlayer = m_mediaPlayer1;
    m_standbyPlayer = m_mediaPlayer2;

    // 初始时将待机播放器静音，避免音频重叠 (Qt 6.x 方式)
    if (m_standbyPlayer->audioOutput()) {
        m_standbyPlayer->audioOutput()->setMuted(true);
    } else {
        qDebug() << "WARNING: Standby player has no audio output, cannot mute.";
    }

    // 设置初始可见的视频部件并开始播放活跃播放器
    m_stackedLayout->setCurrentWidget(m_aspectRatioWidget1); // <--- 修改：显示 AspectRatioWidget
    m_activePlayer->play();

    qDebug() << "DEBUG: PlayerPhotoWidget initialized. Active player:" << (m_activePlayer == m_mediaPlayer1 ? "Player 1" : "Player 2");

    // 当前文字居中
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setText(player->getName());

    m_coinsLabel->setCoinsNum(player->getCoins());

    // 布局调整：m_stackedLayout 占据视频位置，m_coinsLabel 和 m_nameLabel 保持不变
    m_mainLayout->addLayout(m_stackedLayout,0,0,4,4); // 堆叠布局占据视频位置
    m_mainLayout->addWidget(m_nameLabel,4,0,1,5);
    m_mainLayout->addWidget(m_coinsLabel,2,3,2,2); // 保持 CoinsWidget 的位置

    setLayout(m_mainLayout);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){
    // 在析构函数中停止所有播放器，释放资源 (Qt 6.x 方式)
    if (m_mediaPlayer1->playbackState() == QMediaPlayer::PlayingState) m_mediaPlayer1->stop();
    if (m_mediaPlayer2->playbackState() == QMediaPlayer::PlayingState) m_mediaPlayer2->stop();
    // AspectRatioWidget 和 QVideoWidget 会被父类（PlayerPhotoWidget）的布局或析构函数自动删除，
    // 或者由 Qt 的对象树机制删除，只要它们有父对象。
}

void PlayerPhotoWidget::setVideoPlaylist(const QList<QUrl>& urls) {
    qDebug() << "WARNING: setVideoPlaylist is not fully supported with double buffering for single video loop. Only the first URL will be used.";
    if (!urls.isEmpty()) {
        m_videoUrl = urls.first();
        m_mediaPlayer1->setSource(m_videoUrl);
        m_mediaPlayer2->setSource(m_videoUrl);
        // 重置播放状态以应用新视频
        m_mediaPlayer1->stop();
        m_mediaPlayer2->stop();
        m_mediaPlayer1->setPosition(0);
        m_mediaPlayer2->setPosition(0);
        m_activePlayer = m_mediaPlayer1;
        m_standbyPlayer = m_mediaPlayer2;
        // 静音待机播放器 (Qt 6.x 方式)
        if (m_standbyPlayer->audioOutput()) {
            m_standbyPlayer->audioOutput()->setMuted(true);
        }
        m_stackedLayout->setCurrentWidget(m_aspectRatioWidget1); // <--- 修改：显示 AspectRatioWidget
        m_activePlayer->play();
        m_isPreloading = false;
    }
}

void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    const QSize allocatedSize = event->size();
    int height = allocatedSize.height();

    // 字体大小仍然根据 PlayerPhotoWidget 的高度计算
    int fontSize = qMax(5, height / 12);
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

    // 无需在此处手动调整视频尺寸，AspectRatioWidget 会自动处理。
    // 但是，m_stackedLayout 所在的网格单元格需要确保能让 AspectRatioWidget 正常工作。
    // QGridLayout 默认会尝试填充所有空间，AspectRatioWidget 会在其内部保持比例。
}

void PlayerPhotoWidget::onCoinsChange(Player *player, int coins){
    if(player!=m_player)
        return;
    m_coinsLabel->setCoinsNum(player->getCoins());
}

void PlayerPhotoWidget::onPlayerPositionChanged(qint64 position) {
    QMediaPlayer* senderPlayer = qobject_cast<QMediaPlayer*>(sender());

    // 只处理当前活跃播放器的位置变化
    if (senderPlayer == m_activePlayer) {
        qint64 duration = m_activePlayer->duration();
        // 定义预加载的阈值（例如，视频结束前 300 毫秒）
        const qint64 PRELOAD_THRESHOLD_MS = 300;

        // 如果视频时长有效，且当前位置达到预加载阈值，并且尚未开始预加载
        if (duration > 0 && position >= duration - PRELOAD_THRESHOLD_MS && !m_isPreloading) {
            // 确保待机播放器已经加载好媒体
            if (m_standbyPlayer->mediaStatus() == QMediaPlayer::LoadedMedia) {
                m_standbyPlayer->setPosition(0); // 将待机播放器位置重置到开头
                m_standbyPlayer->play();         // 开始播放待机播放器（此时仍静音）
                m_isPreloading = true;           // 设置预加载标志，防止重复启动
                qDebug() << "DEBUG: Active player at pre-load threshold. Standby player started pre-playing.";
            } else {
                qDebug() << "WARNING: Standby player not in LoadedMedia state for pre-loading.";
            }
        }
    }
}

void PlayerPhotoWidget::onPlayerMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    QMediaPlayer* senderPlayer = qobject_cast<QMediaPlayer*>(sender());

    qDebug() << "Media Status for" << (senderPlayer == m_mediaPlayer1 ? "Player 1" : "Player 2") << "changed to:" << status;

    // 如果是当前活跃播放器达到了媒体结束状态
    if (senderPlayer == m_activePlayer && status == QMediaPlayer::EndOfMedia) {
        qDebug() << "DEBUG: Active player reached EndOfMedia. Initiating swap.";

        // 确保待机播放器已经处于播放状态 (Qt 6.x 方式)
        if (m_standbyPlayer->playbackState() == QMediaPlayer::PlayingState) {
            // 执行视觉上的切换：将堆叠布局的当前部件切换到待机播放器对应的视频部件
            if (m_activePlayer == m_mediaPlayer1) {
                m_stackedLayout->setCurrentWidget(m_aspectRatioWidget2); // <--- 修改：切换 AspectRatioWidget
                qDebug() << "DEBUG: Switched to VideoWidget 2.";
            } else {
                m_stackedLayout->setCurrentWidget(m_aspectRatioWidget1); // <--- 修改：切换 AspectRatioWidget
                qDebug() << "DEBUG: Switched to VideoWidget 1.";
            }

            // 静音旧的活跃播放器（现在它将成为待机播放器），取消静音新的活跃播放器 (Qt 6.x 方式)
            if (m_activePlayer->audioOutput()) {
                m_activePlayer->audioOutput()->setMuted(true); // 静音
            }
            if (m_standbyPlayer->audioOutput()) {
                m_standbyPlayer->audioOutput()->setMuted(false); // 取消静音
            }

            // 交换活跃播放器和待机播放器的角色
            QMediaPlayer* temp = m_activePlayer;
            m_activePlayer = m_standbyPlayer;
            m_standbyPlayer = temp;

            // 停止并重置旧的活跃播放器（现在是新的待机播放器）
            m_standbyPlayer->stop();
            m_standbyPlayer->setPosition(0);
            m_isPreloading = false; // 重置预加载标志，为下一个循环做准备

            qDebug() << "DEBUG: Players swapped. New active player:" << (m_activePlayer == m_mediaPlayer1 ? "Player 1" : "Player 2");
        } else {
            // 如果待机播放器没有准备好，作为回退方案，直接重置当前播放器并播放
            qDebug() << "WARNING: Standby player not playing at EndOfMedia. Fallback to simple loop for active player.";
            m_activePlayer->setPosition(0);
            m_activePlayer->play();
            m_isPreloading = false; // 即使回退也重置标志
        }
    }
    // 如果待机播放器意外地达到了媒体结束状态（理论上不应该发生，除非切换逻辑有误）
    else if (senderPlayer == m_standbyPlayer && status == QMediaPlayer::EndOfMedia) {
        qDebug() << "WARNING: Standby player reached EndOfMedia unexpectedly. Resetting.";
        m_standbyPlayer->setPosition(0);
        m_isPreloading = false; // 重置标志
        // 此时不应该播放待机播放器，它应该等待被激活
    }
}

void PlayerPhotoWidget::onPlayerErrorOccurred(QMediaPlayer::Error error, const QString &errorString) {
    QMediaPlayer* senderPlayer = qobject_cast<QMediaPlayer*>(sender());
    qDebug() << "Media Player Error for" << (senderPlayer == m_mediaPlayer1 ? "Player 1" : "Player 2") << ":" << error << errorString;
}
