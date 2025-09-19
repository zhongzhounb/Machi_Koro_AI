#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsVideoItem> // <-- 改回 QGraphicsVideoItem
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QGridLayout>

class Player;
class CoinsWidget;
class QLabel;

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
    // void paintEvent(QPaintEvent *event) override; // <-- 移除 paintEvent 声明

private:
    void adjustItemPositions();

    Player* m_player;

    QVBoxLayout* m_mainLayout;

    QGraphicsScene* m_graphicsScene;
    QGraphicsView* m_graphicsView;
    QGraphicsVideoItem* m_videoItem; // <-- 改回 QGraphicsVideoItem

    QMediaPlayer* m_mediaPlayer;

    QWidget* m_textContainerWidget;
    QGridLayout* m_textLayout;
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;

    QGraphicsProxyWidget* m_textProxy;

    // int m_currentBorderWidth; // 不再需要，QSS处理
    // qreal m_borderRadius;     // 不再需要，QSS处理
};

#endif // PLAYERPHOTOWIDGET_H
