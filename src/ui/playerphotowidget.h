#ifndef PLAYERPHOTOWIDGET_H
#define PLAYERPHOTOWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent> // 需要包含此头文件
class CoinsWidget;
class Player;
class PlayerPhotoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerPhotoWidget(Player* player, QWidget* parent = nullptr);
    ~PlayerPhotoWidget();

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void onCoinsChange(Player* player,int coins);

private:
    int m_coinsNum;
    Player* m_player;
    QGridLayout* m_mainLayout;
    QLabel* m_imgLabel;
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;
    double m_aspectRatio = 1.0;
};

#endif // PLAYERPHOTOWIDGET_H
