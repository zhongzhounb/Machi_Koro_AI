#ifndef ROUNDEDVIDEOCLIPEFFECT_H
#define ROUNDEDVIDEOCLIPEFFECT_H

#include <QGraphicsEffect>
#include <QPainterPath>
#include <QPainter>

class RoundedVideoClipEffect : public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit RoundedVideoClipEffect(QObject *parent = nullptr);

    void setBorderRadius(qreal radius);
    qreal borderRadius() const { return m_borderRadius; }

protected:
    void draw(QPainter *painter) override;

private:
    qreal m_borderRadius = 0;
};

#endif // ROUNDEDVIDEOCLIPEFFECT_H
