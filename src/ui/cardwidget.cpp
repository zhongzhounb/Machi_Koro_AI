#include "cardwidget.h"
#include <QDebug>
#include <QColor>
#include <QFile>
#include <QResizeEvent>
#include <QLabel>
#include "card.h"
#include <QGraphicsDropShadowEffect> // 新增：阴影效果头文件
#include <QPainterPath> // 如果未来需要更精细的图片圆角裁剪，可能会用到
#include <QBuffer> // 新增：用于 QPixmap 转 Base64
#include <QStandardPaths> // 用于调试保存图片
#include <QPainter>
// --- 参考尺寸和位置常量 ---
const int CARD_REF_WIDTH = 100;
const int CARD_REF_HEIGHT = 100;

const QRect IMG_RECT(0, 0, CARD_REF_WIDTH, CARD_REF_HEIGHT); // 建筑图片的位置和大小（全屏）
const QRect ACTIVATION_RANGE_RECT(0, 0, CARD_REF_WIDTH, 15);
const QRect NAME_RECT(0, 15, CARD_REF_WIDTH, 20); // x, y, width, height
const QRect COST_RECT(0, 80, CARD_REF_WIDTH, 20);
const QRect DESCRIPTION_RECT(5, 100, CARD_REF_WIDTH, 20);

const int NAME_FONT_SIZE = 10;
const int DESCRIPTION_FONT_SIZE = 7;
// --- 参考尺寸和位置常量结束 ---


//图片圆角算法
QPixmap QPixmapToRound(const QPixmap & img, int radius)
{
    if (img.isNull())
    {
        return QPixmap();
    }
    QSize size(img.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), radius, radius);
    QPixmap image = img;// .scaled(size);
    image.setMask(mask);
    return image;
}

// 辅助函数：根据比例因子缩放 QRect
QRect scaledRect(const QRect& originalRect, qreal scaleX, qreal scaleY) {
    // 使用 qRound 进行四舍五入，避免浮点数截断导致像素偏差
    int x = qRound(originalRect.x() * scaleX);
    int y = qRound(originalRect.y() * scaleY);
    int width = qRound(originalRect.width() * scaleX);
    int height = qRound(originalRect.height() * scaleY);
    return QRect(x, y, width, height);
}

// 辅助函数定义：将 Color 枚举转换为 QColor
QColor colorToQColor(Color color) {
    switch (color) {
    case Color::Landmark: return QColor("#BC4C42");
    case Color::Red:      return QColor("#C33540");
    case Color::Blue:     return QColor("#5771B9");
    case Color::Green:    return QColor("#506A43");
    case Color::Purple:   return QColor("#7A3085");
    default:              return QColor("white");
    }
}

// 辅助函数：将 Color 枚举转换为背景图片路径
QString colorToImagePath(Color color) {
    switch (color) {
    case Color::Landmark: return ":/resources/images/card/background/landmark.png";
    case Color::Red:      return ":/resources/images/card/background/red.png";
    case Color::Blue:     return ":/resources/images/card/background/blue.png";
    case Color::Green:    return ":/resources/images/card/background/green.png";
    case Color::Purple:   return ":/resources/images/card/background/purple.png";
    case Color::BackNone: return ":/resources/images/card/background/backnone.png";
    case Color::BackSmall: return ":/resources/images/card/background/backsmall.png";
    case Color::BackBig: return ":/resources/images/card/background/backbig.png";
    case Color::BackPurple: return ":/resources/images/card/background/backpurple.png";
    default:              return "";
    }
}

// 辅助函数：将卡牌类名转换为建筑图片路径
QString classNameToImagePath(const QString& className) {
    QString path = QString(":/resources/images/card/img/%1.png").arg(className);

#ifdef QT_DEBUG
    if (!QFile::exists(path)) {
        qDebug() << "DEBUG: Image resource file does NOT exist for class:" << className << " at path:" << path;
    }
#endif
    return path;
}

CardWidget::CardWidget(Card* card,ShowType type, QWidget* parent)
    : QFrame(parent)
    , m_card(card)
    , m_type(type)
    , m_backgroundImgLabel(new QLabel(this))
    , m_imgLabel(new QLabel(this))
    , m_nameLabel(new QLabel(this))
    , m_costLabel(new QLabel(this))
    , m_activationRangeLabel(new QLabel(this))
    , m_descriptionLabel(new QLabel(this))
    , m_stateOverlayLabel(new QLabel("CLOSED", this))

{

    // 确保内容填充整个 QFrame，没有额外的边距
    setContentsMargins(0, 0, 0, 0);

    //描述在详情时显示
    m_descriptionLabel->hide();


    initUI(); // 初始化 QLabel 实例
    updateData(); // 填充数据
    updatePosition(); // 初始设置标签位置

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }
}

CardWidget::~CardWidget(){}

void CardWidget::resizeEvent(QResizeEvent *event)
{
    updatePosition();
    QFrame::resizeEvent(event); // 调用基类的 resizeEvent

}

void CardWidget::updatePosition()
{
    qDebug()<<width();
    qDebug()<<height();
    //计算缩放因子
    qreal scaleX = static_cast<qreal>(width()) / CARD_REF_WIDTH;
    qreal scaleY = static_cast<qreal>(height()) / CARD_REF_HEIGHT;
    qDebug()<<scaleX;
    qDebug()<<scaleY;
    qreal fontScale = qMin(scaleX, scaleY);

    //子类缩放
    m_backgroundImgLabel->setGeometry(scaledRect(IMG_RECT,fontScale,fontScale));
    m_imgLabel->setGeometry(scaledRect(IMG_RECT,fontScale,fontScale));
    m_activationRangeLabel->setGeometry(scaledRect(ACTIVATION_RANGE_RECT,fontScale,fontScale));
    m_nameLabel->setGeometry(scaledRect(NAME_RECT,fontScale,fontScale));
    m_descriptionLabel->setGeometry(scaledRect(DESCRIPTION_RECT,fontScale,fontScale));
    m_costLabel->setGeometry(scaledRect(COST_RECT, fontScale, fontScale));
    this->setGeometry(scaledRect(IMG_RECT,fontScale,fontScale));

    //子类字体缩放
    QFont nameFont("YouYuan", NAME_FONT_SIZE * fontScale, QFont::Bold);
    QFont descriptionFont("YouYuan", DESCRIPTION_FONT_SIZE * fontScale);
    m_nameLabel->setFont(nameFont);
    m_costLabel->setFont(nameFont);
    m_activationRangeLabel->setFont(nameFont);
    m_descriptionLabel->setFont(descriptionFont);

}


void CardWidget::initUI()
{
    // 确保图片能缩放
    m_imgLabel->setScaledContents(true); // 确保建筑图片缩放
    m_backgroundImgLabel->setScaledContents(true);

    // 默认隐藏
    m_stateOverlayLabel->hide(); // 默认隐藏

    // 设置背景图片
    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    backgroundPixmap=backgroundPixmap.copy(QRect(0,0,720,720));
    backgroundPixmap=QPixmapToRound(backgroundPixmap,50);
    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(backgroundPixmap);
    } else {
        //qWarning() << "Failed to load background image for color:" << static_cast<int>(m_card->getColor());
        m_backgroundImgLabel->clear(); // 清除图片
    }

    // 设置建筑图片
    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    cardImagePixmap=cardImagePixmap.copy(QRect(0,0+200,1900,1900));
    cardImagePixmap=QPixmapToRound(cardImagePixmap,50);
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(cardImagePixmap);
    } else {
        //qWarning() << "Failed to load card image for class:" << m_card->metaObject()->className() << " Path:" << classNameToImagePath(m_card->metaObject()->className());
        m_imgLabel->clear();
    }

    // 文字上色
    QString textStyle = "QLabel { color:white; }";
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()).name());
    m_activationRangeLabel->setStyleSheet(textStyle);
    m_nameLabel->setStyleSheet(nameStyle);
    m_descriptionLabel->setStyleSheet(textStyle);
    m_costLabel->setStyleSheet(textStyle);

    // 文字布局
    m_activationRangeLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_costLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_descriptionLabel->setWordWrap(true);//文字多需要显示全部

    //有遮挡则在这设置顶
    m_costLabel->raise();


    if(m_type==ShowType::BackGround){
        m_activationRangeLabel->hide();
        m_nameLabel->hide();
        m_descriptionLabel->hide();
        m_costLabel->hide();
        m_imgLabel->hide();
    }

}


void CardWidget::updateData()
{
    if (!m_card) {
        qWarning() << "CardWidget: No card assigned.";
        return;
    }
    //名称为类型与名称合并
    m_nameLabel->setText(typeToImg(m_card->getType()) + m_card->getName());
    //花费显示
    m_costLabel->setText(QString("%1🪙").arg(m_card->getCost()));
    //描述显示
    m_descriptionLabel->setText(m_card->getDescription());
    //如果无范围则隐藏，单范围单显示，多范围范围显示
    if (m_card->getActLNum() == 0 && m_card->getActRNum() == 0)
        m_activationRangeLabel->hide();
    else if (m_card->getActLNum() == m_card->getActRNum())
        m_activationRangeLabel->setText(QString("%1").arg(m_card->getActLNum()));
    else
        m_activationRangeLabel->setText(QString("%1~%2").arg(m_card->getActLNum()).arg(m_card->getActRNum()));
    //如果建筑是停业则显示
    if(m_card->getState()==State::Closing)
        m_stateOverlayLabel->show();
    else
        m_stateOverlayLabel->hide();
}


void CardWidget::onCardStateChanged(Card* card, State newState)
{
    if (card != m_card) return;
    //需要补个高亮特效
    updateData();
}

void CardWidget::onCardValueChanged(Card* card)
{
    if (card != m_card) return;
    updateData();
}

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_card); // 发射点击信号，传递卡牌数据
    }
    QFrame::mousePressEvent(event); // 调用基类的实现
}
