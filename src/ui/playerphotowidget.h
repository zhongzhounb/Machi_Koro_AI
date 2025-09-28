#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer> // 添加QTimer头文件
#include "roundedvideoitem.h" // 包含自定义视频项的头文件

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
    void onCurrentPlayerChanged(Player* currentPlayer);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override; // <-- 保持重写paintEvent

private slots: // 添加私有槽函数
    void animateDashOffset(); // 用于虚线动画

private:
    void adjustItemPositions(); // 辅助函数，用于调整场景中各项的位置和大小

    Player* m_player;

    QVBoxLayout* m_mainLayout; // 主布局，容纳QGraphicsView

    QGraphicsScene* m_graphicsScene;    // 图形场景
    QGraphicsView* m_graphicsView;      // 图形视图
    RoundedVideoItem* m_videoItem;    // 使用RoundedVideoItem*

    QMediaPlayer* m_mediaPlayer;

    // 文本相关的部件和布局
    QWidget* m_textContainerWidget; // 用于封装文本和金币标签的容器
    QGridLayout* m_textLayout;      // 用于m_textContainerWidget的布局
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;

    // 用于存储封装了m_textContainerWidget的QGraphicsProxyWidget
    QGraphicsProxyWidget* m_textProxy;

    int m_currentBorderWidth; // <-- 存储当前边框宽度
    qreal m_borderRadius;     // <-- 存储圆角半径

    // 新增成员变量用于虚线动画
    bool m_isCurrentPlayer;         // 标记是否是当前玩家
    QTimer* m_dashAnimationTimer;   // 虚线动画定时器
    qreal m_dashOffset;             // 虚线偏移量
    QVector<qreal> m_dashPattern;   // 虚线模式 (例如: {dash_length, space_length})
};

#endif // PLAYERPHOTOWIDGET_H
