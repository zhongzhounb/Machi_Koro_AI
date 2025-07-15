#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"
#include <QResizeEvent>
#include <QFont>
#include <QDebug>
#include <QCoreApplication> // 用于获取应用程序路径

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    :QWidget(parent)
    ,m_player(player)
    ,m_mainLayout(new QGridLayout(this))
    ,m_nameLabel(new QLabel(this))
    ,m_videoWidget(new QVideoWidget(this))
    ,m_mediaPlayer(new QMediaPlayer(this))
    ,m_coinsLabel(new CoinsWidget(this))
    ,m_currentMediaIndex(0) // 初始化索引
{
    // 将 QVideoWidget 设置为 QMediaPlayer 的视频输出
    m_mediaPlayer->setVideoOutput(m_videoWidget);

    // 连接媒体播放器状态变化的信号
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &PlayerPhotoWidget::onMediaStatusChanged);
    connect(m_mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString){
        qDebug() << "Media Player Error:" << error << errorString;
    });

    // 设置视频文件路径 (示例)
    // 请替换为您的MP4文件实际路径。
    // 如果文件在资源文件中 (例如: 添加到 .qrc 文件中，路径为 /videos/player_video.mp4)，则使用 ":/videos/player_video.mp4"
    // 如果文件在应用程序可执行文件同级目录或特定路径，则使用 "path/to/your/player_video.mp4"
    // 为了演示循环播放，这里添加同一个视频两次
    QString videoResourcePath = "qrc:/resources/images/player/video/Amy/ordinary.mp4"; // <--- 修改为您的实际资源路径

    m_playlist.append(QUrl(videoResourcePath));
    // 如果您想让单个视频循环播放，可以只添加一次，并在 onMediaStatusChanged 中重置索引
    // 或者，如果您有多个视频，可以依次添加：
    // m_playlist.append(QUrl::fromLocalFile("path/to/your/video2.mp4"));
    // m_playlist.append(QUrl::fromLocalFile("path/to/your/video3.mp4"));
    qDebug() << "DEBUG: Playlist size after append:" << m_playlist.size();
    if (!m_playlist.isEmpty()) {
        qDebug() << "DEBUG: First item in playlist (toString):" << m_playlist.first().toString();
        qDebug() << "DEBUG: First item in playlist (isValid):" << m_playlist.first().isValid();
        qDebug() << "DEBUG: First item in playlist (isLocalFile):" << m_playlist.first().isLocalFile();
        qDebug() << "DEBUG: First item in playlist (scheme):" << m_playlist.first().scheme();
        qDebug() << "DEBUG: First item in playlist (path):" << m_playlist.first().path();
    }

    // 初始播放第一个视频
    playNextMedia();

    //当前文字居中
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setText(player->getName());

    m_coinsLabel->setCoinsNum(player->getCoins());

    // 将 m_imgLabel 替换为 m_videoWidget
    m_mainLayout->addWidget(m_videoWidget,0,0,4,4); // 视频部件占据原来图片的位置
    m_mainLayout->addWidget(m_nameLabel,4,0,1,5);
    m_mainLayout->addWidget(m_coinsLabel,2,3,2,2);

    setLayout(m_mainLayout);
}

PlayerPhotoWidget::~PlayerPhotoWidget(){}

void PlayerPhotoWidget::setVideoPlaylist(const QList<QUrl>& urls) {
    m_playlist = urls;
    m_currentMediaIndex = 0;
    playNextMedia(); // 设置新播放列表后立即播放第一个
}

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

void PlayerPhotoWidget::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        qDebug() << "End of media reached. Playing next...";
        playNextMedia();
    } else {
        qDebug() << "Media Status:" << status;
    }
}

void PlayerPhotoWidget::playNextMedia() {
    if (m_playlist.isEmpty()) {
        qDebug() << "Playlist is empty.";
        m_mediaPlayer->stop();
        return;
    }

    // 循环播放逻辑：如果到达列表末尾，则重置回第一个
    if (m_currentMediaIndex >= m_playlist.size()) {
        m_currentMediaIndex = 0;
    }

    QUrl mediaUrl = m_playlist.at(m_currentMediaIndex);
    qDebug() << "DEBUG: In playNextMedia - currentMediaIndex:" << m_currentMediaIndex;
    qDebug() << "DEBUG: In playNextMedia - mediaUrl (toString):" << mediaUrl.toString();
    qDebug() << "DEBUG: In playNextMedia - mediaUrl (isValid):" << mediaUrl.isValid();
    qDebug() << "DEBUG: In playNextMedia - mediaUrl (isLocalFile):" << mediaUrl.isLocalFile();
    qDebug() << "DEBUG: In playNextMedia - mediaUrl (scheme):" << mediaUrl.scheme();
    qDebug() << "DEBUG: In playNextMedia - mediaUrl (path):" << mediaUrl.path();

    m_mediaPlayer->setSource(mediaUrl); // 使用 setSource 替代 setMedia
    m_mediaPlayer->play();

    m_currentMediaIndex++; // 准备播放下一个
}
