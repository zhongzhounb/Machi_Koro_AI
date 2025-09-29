#ifndef ROUNDEDVIDEOITEM_H
#define ROUNDEDVIDEOITEM_H

#include <QGraphicsVideoItem>
#include <QPainterPath> // 用于定义裁剪路径

class RoundedVideoItem : public QGraphicsVideoItem
{
public:
    explicit RoundedVideoItem(QGraphicsItem *parent = nullptr);

    // 设置圆角半径
    void setBorderRadius(qreal radius);

protected:
    // 重写paint方法，在绘制视频前应用裁剪路径
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    qreal m_borderRadius; // 存储圆角半径
};

#endif // ROUNDEDVIDEOITEM_H
