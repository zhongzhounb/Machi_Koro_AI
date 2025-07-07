#include "cardwidget.h"
#include <QDebug>
#include <QColor>
#include <QFile>
#include <QResizeEvent>
#include <QLabel>
#include "card.h"
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QBuffer>
#include <QStandardPaths>
#include <QPainter>
#include <QVBoxLayout> // 确保包含 QVBoxLayout

// --- 参考尺寸和位置常量 --- (保持不变)
const int CARD_REF_WIDTH = 100;
const int CARD_REF_HEIGHT = 100;

const QRect IMG_RECT(0, 0, CARD_REF_WIDTH, CARD_REF_HEIGHT);
const QRect ACTIVATION_RANGE_RECT(0, 0, CARD_REF_WIDTH, 15);
const QRect NAME_RECT(0, 15, CARD_REF_WIDTH, 20);
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

// CardWidget 构造函数
CardWidget::CardWidget(Card* card,ShowType type, QWidget* parent)
    : QFrame(parent) // CardWidget 继承 QFrame
    , m_card(card)
    , m_type(type)
    // 1. 创建 m_cardContentContainer 作为所有内容的父容器
    , m_cardContentContainer(new QWidget(this)) // 父对象是 CardWidget
    // 2. 将 m_mainLayout 设置给 m_cardContentContainer
    , m_mainLayout(new QStackedLayout(m_cardContentContainer))
    // 3. 所有 QLabel 及其子类的父对象都设置为 m_cardContentContainer
    , m_backgroundImgLabel(new QLabel(m_cardContentContainer))
    , m_imgLabel(new QLabel(m_cardContentContainer))
    , m_textContainer(new QWidget(m_cardContentContainer)) // 文本容器的父对象是 m_cardContentContainer
    , m_activationRangeLabel(new AutoFitTextLabel(m_textContainer)) // 文本标签的父对象是 m_textContainer
    , m_nameLabel(new AutoFitTextLabel(m_textContainer))
    , m_costLabel(new AutoFitTextLabel(m_textContainer))
    , m_descriptionLabel(new QLabel(m_textContainer))
    , m_stateOverlayLabel(new QLabel("CLOSED", m_cardContentContainer))
{
    // 隐藏选择和描述显示（保持不变）
    if(m_type==ShowType::BackGround){
        m_activationRangeLabel->hide();
        m_nameLabel->hide();
        m_descriptionLabel->hide();
        m_costLabel->hide();
        m_imgLabel->hide();
    }
    m_descriptionLabel->hide();

    // 确保 CardWidget 自身内容填充，没有额外边距
    setContentsMargins(0, 0, 0, 0);

    // 4. 创建 AspectRatioWidget，包装 m_cardContentContainer
    // 假设卡牌的理想比例是 1:1.4 （例如，宽100，高140）
    // 你提供的 AspectRatioWidget 构造函数是 AspectRatioWidget(QWidget *widget, float width, float height, QWidget *parent = 0);
    // 这里的 width 和 height 是指理想比例，不是像素值。
    // 你的常量 CARD_REF_WIDTH 和 CARD_REF_HEIGHT 都是 100，这表示 1:1 比例。
    // 如果你的卡牌是正方形，则使用 100, 100。
    // 如果是长方形，例如 70宽 x 100高，则使用 70, 100。
    // 我这里假设你希望卡牌是正方形，所以使用 CARD_REF_WIDTH, CARD_REF_HEIGHT
    m_aspectRatioWrapper = new AspectRatioWidget(m_cardContentContainer, (float)1, (float)1, parent);

    // 5. 将 AspectRatioWidget 设置为 CardWidget 的主布局内容
    // CardWidget 自身使用一个简单的 QVBoxLayout 来包含 m_aspectRatioWrapper
    QVBoxLayout* cardWidgetMainLayout = new QVBoxLayout(this);
    cardWidgetMainLayout->addWidget(m_aspectRatioWrapper);
    cardWidgetMainLayout->setContentsMargins(0,0,0,0); // 移除 CardWidget 自身的边距
    setLayout(cardWidgetMainLayout); // CardWidget 的布局

    initUI(); // 初始化所有 QLabel 实例
    updateData(); // 填充数据

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }
    // 注意：这里移除了 setLayout(m_mainLayout); 因为 m_mainLayout 已经是 m_cardContentContainer 的布局了
}

CardWidget::~CardWidget(){}

void CardWidget::initUI()
{
    // 设置层数全显示
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);

    // 确保图片能缩放 (保持不变)
    m_imgLabel->setScaledContents(true);
    m_backgroundImgLabel->setScaledContents(true);

    // 设置背景图片 (保持不变)
    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    backgroundPixmap=backgroundPixmap.copy(QRect(0,0,720,720)); // 裁剪
    backgroundPixmap=QPixmapToRound(backgroundPixmap,50); // 圆角

    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(backgroundPixmap);
    }

    // 设置建筑图片 (保持不变)
    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    cardImagePixmap=cardImagePixmap.copy(QRect(0,0+200,1900,1900)); // 裁剪
    cardImagePixmap=QPixmapToRound(cardImagePixmap,50); // 圆角
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(cardImagePixmap);
    }

    // 文字上色 (保持不变)
    QString textStyle = "QLabel { color:white; }";
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()).name());
    m_activationRangeLabel->setStyleSheet(textStyle);
    m_nameLabel->setStyleSheet(nameStyle);
    m_descriptionLabel->setStyleSheet(textStyle);
    m_costLabel->setStyleSheet(textStyle);

    // 文字布局 (保持不变)
    m_activationRangeLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_costLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    //m_descriptionLabel->setWordWrap(true); // 描述文本允许换行，AutoFitTextLabel 不支持，QLabel 支持

    // 确保所有层容器都是透明的，以便下层内容透出
    m_imgLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_imgLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    m_textContainer->setAttribute(Qt::WA_TranslucentBackground);
    m_textContainer->setStyleSheet("background-color: rgba(0,0,0,0);");

    m_stateOverlayLabel->setAttribute(Qt::WA_TranslucentBackground); // 覆盖层图片如果自带透明度，需要这个
    QColor overlayColor(0, 0, 0, 150); // 黑色，半透明 (alpha 150/255)
    m_stateOverlayLabel->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4); color: white; font-weight: bold; font-size: 20px;")
                                           .arg(overlayColor.red()).arg(overlayColor.green())
                                           .arg(overlayColor.blue()).arg(overlayColor.alpha()));

    // 重新调整添加顺序，确保层级正确
    // QStackedLayout::StackAll 模式下，后添加的在上面
    // 顺序：背景 -> 建筑图 -> 文本 -> 覆盖层
    m_mainLayout->addWidget(m_backgroundImgLabel); // Layer 1: 背景图
    m_mainLayout->addWidget(m_imgLabel);           // Layer 2: 建筑图

    // 第三层：文字层
    // m_textLayout 已经设置给 m_textContainer，所以这里添加 m_textContainer
    m_textLayout = new QVBoxLayout(m_textContainer); // 布局的父对象是 m_textContainer
    m_textLayout->addWidget(m_activationRangeLabel,1);
    m_textLayout->addWidget(m_nameLabel,1);
    m_textLayout->addStretch(2); // 隔层图片
    m_textLayout->addWidget(m_costLabel,1);
    m_mainLayout->addWidget(m_textContainer); // 添加文本容器

    // 第四层：覆盖层
    m_mainLayout->addWidget(m_stateOverlayLabel);

    // 初始状态：覆盖层默认隐藏
    m_stateOverlayLabel->hide(); // 初始隐藏覆盖层
}

void CardWidget::updateData()
{
    if (!m_card) {
        qWarning() << "CardWidget: No card assigned.";
        return;
    }
    // 名称为类型与名称合并 (保持不变)
    m_nameLabel->setText(typeToImg(m_card->getType()) + m_card->getName());
    // 花费显示 (保持不变)
    m_costLabel->setText(QString("%1🪙").arg(m_card->getCost()));
    // 描述显示 (保持不变)
    m_descriptionLabel->setText(m_card->getDescription());
    // 激活范围显示 (保持不变)
    if (m_card->getActLNum() == 0 && m_card->getActRNum() == 0)
        m_activationRangeLabel->hide();
    else if (m_card->getActLNum() == m_card->getActRNum())
        m_activationRangeLabel->setText(QString("%1").arg(m_card->getActLNum()));
    else
        m_activationRangeLabel->setText(QString("%1~%2").arg(m_card->getActLNum()).arg(m_card->getActRNum()));

    // 建筑图片和背景图片根据卡牌数据更新
    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    backgroundPixmap = backgroundPixmap.copy(QRect(0,0,720,720));
    backgroundPixmap = QPixmapToRound(backgroundPixmap,50);
    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(backgroundPixmap);
    }

    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    cardImagePixmap = cardImagePixmap.copy(QRect(0,0+200,1900,1900));
    cardImagePixmap = QPixmapToRound(cardImagePixmap,50);
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(cardImagePixmap);
    }

    // 文字颜色更新 (如果颜色是动态的)
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()).name());
    m_nameLabel->setStyleSheet(nameStyle);

    // 停业状态显示 (保持不变)
    if(m_card->getState()==State::Closing)
        m_stateOverlayLabel->show();
    else
        m_stateOverlayLabel->hide();
}

void CardWidget::updatePosition() {
    // AspectRatioWidget 和 QStackedLayout 已经处理了所有布局和位置
    // 这个函数现在可以为空或移除
}

// CardWidget::resizeEvent 可以移除，因为 AspectRatioWidget 已经处理了内容的比例
// void CardWidget::resizeEvent(QResizeEvent *event) {
//     QFrame::resizeEvent(event);
// }

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_card);
    }
    QFrame::mousePressEvent(event); // 调用基类的实现
}

void CardWidget::onCardStateChanged(Card* card, State newState)
{
    if (card != m_card) return;
    // 需要补个高亮特效
    updateData();
}

void CardWidget::onCardValueChanged(Card* card)
{
    if (card != m_card) return;
    updateData();
}
