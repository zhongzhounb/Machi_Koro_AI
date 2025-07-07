#ifndef CARDWIDGET_H
#define CARDWIDGET_H
#include "global.h"
#include <QFrame>
#include <QLabel>
#include <QMouseEvent> // 引入 QMouseEvent
#include <QStackedLayout>
#include <autofittextlabel.h>
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
    //数据
    Card* m_card;
    //显示模式
    ShowType m_type;
    //布局
    QStackedLayout* m_mainLayout;//主布局
    QLabel* m_backgroundImgLabel;//1.背景层
    QLabel* m_imgLabel;//2.建筑图片层
    QWidget* m_textContainer;//3.文本层（含多个文本)
    QLabel* m_stateOverlayLabel;//4.覆盖层

    //文本层
    QVBoxLayout* m_textLayout;//垂直布局
    AutoFitTextLabel* m_activationRangeLabel;//3.1激活范围
    AutoFitTextLabel* m_nameLabel;//3.2建筑名字
    AutoFitTextLabel* m_costLabel;//3.3花费
    QLabel* m_descriptionLabel;//3.4描述（可能需要多个AutoFitTextLabel，目前没做）

    void initUI();
    void updateData();
};

#endif // CARDWIDGET_H
