#ifndef SKYPAINTER_H
#define SKYPAINTER_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QLinearGradient>
#include <QPointF>
#include <QVector>
#include <QPainterPath>
#include <QtMath> // 用于 qFuzzyCompare
#include <QRectF> // 用于卡牌区域

#include "GameBackgroundWidget.h" // 包含主Widget的头文件以访问其枚举
#include "randomutils.h"          // 包含您的随机数工具类

class SkyPainter : public QObject
{
    Q_OBJECT

    // 定义Q_PROPERTY，使其可以通过QPropertyAnimation进行动画
    Q_PROPERTY(QColor skyTopColor READ skyTopColor WRITE setSkyTopColor NOTIFY skyTopColorChanged)
    Q_PROPERTY(QColor skyBottomColor READ skyBottomColor WRITE setSkyBottomColor NOTIFY skyBottomColorChanged)
    Q_PROPERTY(QColor sunMoonColor READ sunMoonColor WRITE setSunMoonColor NOTIFY sunMoonColorChanged)
    Q_PROPERTY(QColor cloudColor READ cloudColor WRITE setCloudColor NOTIFY cloudColorChanged)
    Q_PROPERTY(QColor starColor READ starColor WRITE setStarColor NOTIFY starColorChanged)
    Q_PROPERTY(QPointF sunMoonPosition READ sunMoonPosition WRITE setSunMoonPosition NOTIFY sunMoonPositionChanged)
    Q_PROPERTY(qreal cloudBaseOffsetX READ cloudBaseOffsetX WRITE setCloudBaseOffsetX NOTIFY cloudBaseOffsetXChanged) // 云朵基准偏移属性
    Q_PROPERTY(QRectF cardRect READ cardRect WRITE setCardRect NOTIFY cardRectChanged) // 新增：卡牌区域属性

public:
    explicit SkyPainter(QObject *parent = nullptr);

    // Getter方法
    QColor skyTopColor() const { return m_skyTopColor; }
    QColor skyBottomColor() const { return m_skyBottomColor; }
    QColor sunMoonColor() const { return m_sunMoonColor; }
    QColor cloudColor() const { return m_cloudColor; }
    QColor starColor() const { return m_starColor; }
    QPointF sunMoonPosition() const { return m_sunMoonPosition; }
    qreal cloudBaseOffsetX() const { return m_cloudBaseOffsetX; } // 云朵基准偏移 getter
    QRectF cardRect() const { return m_cardRectRel; } // 新增：卡牌区域 getter

    // Setter方法
    void setSkyTopColor(const QColor& color);
    void setSkyBottomColor(const QColor& color);
    void setSunMoonColor(const QColor& color);
    void setCloudColor(const QColor& color);
    void setStarColor(const QColor& color);
    void setSunMoonPosition(const QPointF& pos);
    void setCloudBaseOffsetX(qreal offset); // 云朵基准偏移 setter
    void setCardRect(const QRectF& rect); // 新增：卡牌区域 setter

    // 设置绘图区域大小
    void setSize(const QSize& size);
    // 设置当前背景状态，以便绘制正确的太阳/月亮/星星（形状/可见性）
    void setBackgroundState(GameBackgroundWidget::BackgroundState state);

    // 执行绘图操作
    void paint(QPainter* painter);

signals:
    // NOTIFY信号
    void skyTopColorChanged();
    void skyBottomColorChanged();
    void sunMoonColorChanged();
    void cloudColorChanged();
    void starColorChanged();
    void sunMoonPositionChanged();
    void cloudBaseOffsetXChanged(); // 云朵基准偏移信号
    void cardRectChanged(); // 新增：卡牌区域改变信号

private:
    QSize m_size;                               // 绘图区域大小
    GameBackgroundWidget::BackgroundState m_currentState; // 当前背景状态

    // 动画属性的实际存储
    QColor m_skyTopColor;
    QColor m_skyBottomColor;
    QColor m_sunMoonColor;
    QColor m_cloudColor;
    QColor m_starColor;
    QPointF m_sunMoonPosition;
    qreal m_cloudBaseOffsetX = 0.0; // 云朵基准水平偏移成员变量
    QRectF m_cardRectRel; // 卡牌在Widget中的相对位置和大小 (0.0 到 1.0)

    // 存储生成的星星位置
    QVector<QPointF> m_starPositions;

    // CloudPart 结构体
    struct CloudPart {
        qreal relX, relY, relW, relH; // 相对位置和大小
    };

    // 用于存储每朵云的速度因子和初始随机偏移
    struct CloudData {
        qreal speedFactor;        // 相对速度因子 (例如 0.8, 1.0, 1.2)
        qreal initialOffsetX;     // 初始随机偏移，让云朵在开始时就分散开
        QVector<CloudPart> templateParts; // 存储这组云朵的形状模板
    };
    QVector<CloudData> m_cloudData; // 用于存储所有云朵的数据

    void initializeCloudData(); // 初始化云朵数据
    void generateStarPositions(); // 新增：生成星星位置的方法

    // 绘制太阳或月亮
    void drawSunMoon(QPainter* painter);
    // 绘制星星
    void drawStars(QPainter* painter);
    // 绘制云朵
    void drawClouds(QPainter* painter);
};

#endif // SKYPAINTER_H
