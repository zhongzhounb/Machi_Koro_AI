#include "diceareawidget.h"
#include "dice.h"
#include "dicewidget.h"
DiceAreaWidget::DiceAreaWidget(Dice* dice,QWidget* parent)
    :QWidget(parent),
    m_dice(dice),
    m_mainLayout(new QHBoxLayout(this)),
    m_diceWidget1(new DiceWidget(this)),
    m_diceWidget2(new DiceWidget(this)),
    m_textLabel(new QLabel(this)){
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    m_mainLayout->addWidget(m_diceWidget1);
    m_mainLayout->addWidget(m_diceWidget2);
    m_mainLayout->addWidget(m_textLabel);

    connect(m_dice,&Dice::dicesChanged,this,&DiceAreaWidget::onDicesChanged);

    onDicesChanged(m_dice);
};
DiceAreaWidget::~DiceAreaWidget(){};

void DiceAreaWidget::onDicesChanged(Dice* dice){
    if(dice!=m_dice)return;

    if(dice->getFirstNum()==0)
        m_diceWidget1->hide();
    else{
        m_diceWidget1->setDiceNum(dice->getFirstNum());
        m_diceWidget1->show();
    }


    if(dice->getSecondNum()==0)
        m_diceWidget2->hide();
    else{
        m_diceWidget2->setDiceNum(dice->getSecondNum());
        m_diceWidget2->show();
    }

    m_textLabel->setText(QString("%1").arg(dice->getSum()));
    m_textLabel->hide();//比例不对之后显示

}

