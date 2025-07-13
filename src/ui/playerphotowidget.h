/*#ifndef PLAYERPHOTOWIDGET_H
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
    // 重写 resizeEvent 以自我调整尺寸和宽高比
    void resizeEvent(QResizeEvent *event) override;

private:
    int m_coinsNum;
    QGridLayout* m_mainLayout;
    QLabel* m_imgLabel;
    QLabel* m_nameLabel;
    CoinsWidget* m_coinsLabel;
};


#endif // PLAYERPHOTOWIDGET_H
*/
