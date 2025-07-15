#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QStackedLayout>
#include <QUrl>
#include <QAudioOutput>
#include "aspectratiowidget.h" // <--- 新增：包含 AspectRatioWidget 头文件

class Player;
class CoinsWidget;

class PlayerPhotoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPhotoWidget(Player* player, QWidget* parent = nullptr);
    ~PlayerPhotoWidget();

    void setVideoPlaylist(const QList<QUrl>& urls);

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void onCoinsChange(Player *player, int coins);

private slots:
    void onPlayerMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlayerPositionChanged(qint64 position);
    void onPlayerErrorOccurred(QMediaPlayer::Error error, const QString &errorString);

private:
    Player* m_player;
    QGridLayout* m_mainLayout;
    QLabel* m_nameLabel;

    QVideoWidget* m_videoWidget1; // 仍然需要，因为是 QMediaPlayer 的输出
    QVideoWidget* m_videoWidget2; // 仍然需要

    AspectRatioWidget* m_aspectRatioWidget1; // <--- 新增：包装 QVideoWidget1
    AspectRatioWidget* m_aspectRatioWidget2; // <--- 新增：包装 QVideoWidget2

    QMediaPlayer* m_mediaPlayer1;
    QMediaPlayer* m_mediaPlayer2;

    QStackedLayout* m_stackedLayout;

    CoinsWidget* m_coinsLabel;

    QMediaPlayer* m_activePlayer;
    QMediaPlayer* m_standbyPlayer;

    QUrl m_videoUrl;

    bool m_isPreloading;
};

#endif // PLAYERPHOTOWIDGET_H
