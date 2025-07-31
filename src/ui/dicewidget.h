#ifndef DICEWIDGET_H
#define DICEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QPixmap> // 包含 QPixmap 头文件

class DiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiceWidget(QWidget* parent = nullptr, int diceNum = 1);
    ~DiceWidget();

    void setDiceNum(int diceNum);
    int getDiceNum() const { return m_diceNum; }

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateLabelGeometry(); // 辅助函数，用于更新标签几何形状和图像

    int m_diceNum;
    float m_aspectRatio;
    QVector<QLabel*> m_imgLabels;
    QVector<QPixmap> m_originalPixmaps; // *** 新增：存储原始高质量图片 ***
};

#endif // DICEWIDGET_H
