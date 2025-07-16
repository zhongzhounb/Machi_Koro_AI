#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QGraphicsProxyWidget> // 新增
#include <QVBoxLayout> // 用于主布局
#include <QGridLayout> // 用于文本容器的内部布局

class Player; // 前向声明
class CoinsWidget; // 前向声明
class QLabel; // 确保包含QLabel的定义

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
    void adjustItemPositions(); // 辅助函数，用于调整场景中各项的位置和大小

    Player* m_player;

    QVBoxLayout* m_mainLayout; // 主布局，容纳QGraphicsView

    QGraphicsScene* m_graphicsScene;    // 图形场景
    QGraphicsView* m_graphicsView;      // 图形视图
    QGraphicsVideoItem* m_videoItem;    // 视频项

    QMediaPlayer* m_mediaPlayer;

    // 文本相关的部件和布局
    QWidget* m_textContainerWidget; // 新增：用于封装文本和金币标签的容器
    QGridLayout* m_textLayout;      // 新增：用于m_textContainerWidget的布局
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;

    // 新增：用于存储封装了m_textContainerWidget的QGraphicsProxyWidget
    QGraphicsProxyWidget* m_textProxy;
};

#endif // PLAYERPHOTOWIDGET_H
