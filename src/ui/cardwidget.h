#ifndef CARDWIDGET_H
#define CARDWIDGET_H
#include "global.h"
#include <QFrame>
#include <QLabel>
#include <QMouseEvent> // 引入 QMouseEvent
class Card;

// 假设这些辅助函数在某个公共头文件或cardwidget.cpp中
QString typeToImg(Type type);
QColor colorToQColor(Color color);
QString colorToImagePath(Color color);
QString classNameToImagePath(const QString& className);

class CardWidget : public QFrame
{
    Q_OBJECT
public:


    explicit CardWidget(Card* card,ShowType type=ShowType::Ordinary, QWidget* parent = nullptr);
    ~CardWidget();

    Card* getCard() const { return m_card; } // 添加一个获取Card指针的方法

signals:
    void clicked(Card* card); // 当卡牌被点击时发出信号

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override; // 添加鼠标点击事件处理

private slots:
    void onCardStateChanged(Card* card, State newState);
    void onCardValueChanged(Card* card);

private:
    Card* m_card;
    ShowType m_type;
    QLabel* m_backgroundImgLabel;
    QLabel* m_nameLabel;
    QLabel* m_costLabel;
    QLabel* m_activationRangeLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_stateOverlayLabel;
    QLabel* m_imgLabel;

    void initUI();
    void updateData();
    void updatePosition();
};

#endif // CARDWIDGET_H
