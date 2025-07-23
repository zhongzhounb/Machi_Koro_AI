#include "dicewidget.h"

DiceWidget::DiceWidget(QWidget* parent,int diceNum,int color)
    :QWidget(parent),
    m_diceNum(diceNum),
    m_color(color),
    m_mainLayout(new QStackedLayout(this)),
    m_textLabel(new QLabel(this)){
    //全显示
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);
    //当前文字居中
    m_textLabel->setAlignment(Qt::AlignCenter);
    setDiceNum(m_diceNum);
    //设置样式
    m_textLabel->setStyleSheet("QLabel { background:green;color:white; }");

    m_mainLayout->addWidget(m_textLabel);

    setLayout(m_mainLayout);
};
DiceWidget::~DiceWidget(){};

void DiceWidget::setDiceNum(int diceNum){
    m_diceNum=diceNum;
    QString diceNumIcon;
    switch(diceNum){
    case 1:diceNumIcon="1";break;//这个icon太小了，回去换个
    case 2:diceNumIcon="2";break;
    case 3:diceNumIcon="3";break;
    case 4:diceNumIcon="4";break;
    case 5:diceNumIcon="5";break;
    case 6:diceNumIcon="6";break;
    default:diceNumIcon="?";
    }

    m_textLabel->setText(diceNumIcon);
}

// 重写 resizeEvent 以自我调整尺寸和宽高比
void DiceWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    int fontSize = qMax(5, height/2);
    m_textLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
};
