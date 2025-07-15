#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QList> // 新增，用于存储URL列表
#include <QUrl>  // 新增，用于处理URL

class Player; // 前向声明
class CoinsWidget; // 前向声明

class PlayerPhotoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerPhotoWidget(Player* player, QWidget* parent = nullptr);
    ~PlayerPhotoWidget();

    // 可以添加一个方法来设置视频列表，如果需要动态更改
    void setVideoPlaylist(const QList<QUrl>& urls);

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void onCoinsChange(Player *player, int coins);

private slots:
    // 槽函数，用于处理媒体状态变化，特别是视频播放结束
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    void playNextMedia(); // 辅助函数，用于播放列表中的下一个媒体

    Player* m_player;
    QGridLayout* m_mainLayout;
    QLabel* m_nameLabel;
    QVideoWidget* m_videoWidget;
    QMediaPlayer* m_mediaPlayer;
    CoinsWidget* m_coinsLabel;

    QList<QUrl> m_playlist; // 播放列表
    int m_currentMediaIndex; // 当前播放的媒体索引
};

#endif // PLAYERPHOTOWIDGET_H
