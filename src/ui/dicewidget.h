#ifndef DICEWIDGET_H
#define DICEWIDGET_H
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent> // 需要包含此头文件
class DiceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DiceWidget( QWidget* parent = nullptr,int diceNum=0);
    ~DiceWidget();

public slots:
    void setDiceNum(int diceNum);

protected:
    // 重写 resizeEvent 以自我调整尺寸和宽高比
    //void resizeEvent(QResizeEvent *event) override;

private:
    int m_diceNum;
    QStackedLayout* m_mainLayout;
    QList<QLabel*> m_imgLabels;
    double m_aspectRatio=1.0;
    bool m_isResizing=false;
};
#endif // DICEWIDGET_H
