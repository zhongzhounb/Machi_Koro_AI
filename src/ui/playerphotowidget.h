#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QStackedLayout>
#include <QUrl> // 确保包含 QUrl
// QStackedLayout 在 .cpp 中使用，如果这里不直接声明成员变量，则不需要在这里包含或前向声明

// 前向声明 Player 类，避免循环依赖
class Player;
class CoinsWidget; // 前向声明 CoinsWidget

class PlayerPhotoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPhotoWidget(Player* player, QWidget* parent = nullptr);
    ~PlayerPhotoWidget();

public slots:
    void onCoinsChange(Player* player, int coins);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Player* m_player;
    QStackedLayout* m_mainLayout; // 主布局（叠加布局）

    //第一层视频层
    QWidget* m_videoContainer;
    QGridLayout* m_videoLayout;
    QVideoWidget* m_videoWidget;
    QMediaPlayer* m_mediaPlayer;

    //第二层遮罩视频层（之后会添加）

    //第三层文字层
    QWidget* m_textContainer;
    QGridLayout* m_textLayout;
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;

};

#endif // PLAYERPHOTOWIDGET_H
