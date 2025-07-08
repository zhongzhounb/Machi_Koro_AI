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
    , m_mainLayout(new QStackedLayout(this)) // 主布局，父对象是 CardWidget
    , m_backgroundImgLabel(new QLabel(this)) // Layer 1
    , m_imgLabel(new QLabel(this))           // Layer 2
    , m_textContainer(new QWidget(this))     // Layer 3
    , m_activationRangeLabel(new QLabel(this))// Layer 3.1
    , m_nameLabel(new QLabel(this))// Layer 3.2
    , m_descriptionLabel(new QLabel(this))// Layer 3.3
    , m_costLabel(new QLabel(this))// Layer 3.4
    , m_stateOverlayLabel(new QLabel("CLOSED", this))  // Layer 4

{
    //隐藏选择
    if(m_type==ShowType::BackGround){
        m_activationRangeLabel->hide();
        m_nameLabel->hide();
        m_descriptionLabel->hide();
        m_costLabel->hide();
        m_imgLabel->hide();
    }
    //描述在详情时显示
    m_descriptionLabel->hide();

    // 确保内容填充整个 QFrame，没有额外的边距
    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));

    initUI(); // 初始化 QLabel 实例
    updateData(); // 填充数据

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }

    setLayout(m_mainLayout);
}

CardWidget::~CardWidget(){}



void CardWidget::initUI()
{

    //设置层数全显示
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);

    // 确保图片能缩放
    m_imgLabel->setScaledContents(true);
    m_backgroundImgLabel->setScaledContents(true);

    // 设置背景图片
    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    backgroundPixmap=backgroundPixmap.copy(QRect(0,0,720,720));//裁剪
    backgroundPixmap=QPixmapToRound(backgroundPixmap,50);//园角

    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(backgroundPixmap);
    }

    // 设置建筑图片
    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    cardImagePixmap=cardImagePixmap.copy(QRect(0,0+200,1900,1900));//裁剪
    cardImagePixmap=QPixmapToRound(cardImagePixmap,50);//园角
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(cardImagePixmap);
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
    //m_descriptionLabel->setWordWrap(true);以后需要处理m_descriptionLabel分段问题

    //第四层：覆盖层
    m_mainLayout->addWidget(m_stateOverlayLabel);

    //第三层：文字层
    m_textLayout = new QVBoxLayout(m_textContainer);
    m_textLayout->addWidget(m_activationRangeLabel,1); //激活
    m_textLayout->addWidget(m_nameLabel,1);//名字
    m_textLayout->addStretch(2);//隔层图片
    m_textLayout->addWidget(m_costLabel,1);//花费
    m_mainLayout->addWidget(m_textContainer);

    //第二层：建筑图
    m_mainLayout->addWidget(m_imgLabel);

    //第一层：背景图
    m_mainLayout->addWidget(m_backgroundImgLabel);

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
    // 调用基类的实现
}
