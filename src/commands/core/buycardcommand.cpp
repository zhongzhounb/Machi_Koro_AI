#include "buycardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "cardstore.h"


BuyCardCommand::BuyCardCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::RollDice, sourcePlayer,parent){
}


// 检查是否需要用户交互（默认不需要交互）
// 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
PromptData CloseLandmarkCommand::getPromptData(GameState* state) const {

};
// 获取默认选项（无选项时禁止调用）
int CloseLandmarkCommand::getAutoInput( const PromptData& promptData ,GameState* state) const {

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool CloseLandmarkCommand::setInput(int optionId,GameState* state) {

};


void BuyCardCommand::execute(GameState* state, GameController* controller){
    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();
    int cardId=chooes.at(0).toInt();
    //如果啥也没买
    if(cardId==0){
        m_isFailed=true;
        return ;
    }
    //买了的话得取消机场效果
    if(m_sourcePlayer->getCardNum("机场",State::Opening)>0){
        QList<GameCommand*> commands=controller->getCommands(CommandType::GainCoinsIfNoBuyCard);
        if(commands.size()>0)
            controller->delCommand(commands.at(0));
        //其他情况不变
    }
    //创建命令，如果有科技公司
    if(m_sourcePlayer->getCardNum("科技公司",State::Opening)>0)
        controller->addCommand(m_sourcePlayer->getCardSpecialCommand("科技公司"));
    //如果是建地标建筑
    for(QList<Card*>cards:m_sourcePlayer->getCards())
        if(cards.first()->getId()==cardId){
            Card* card=cards.first();
            m_cardName=card->getName();
            m_cardCoins=card->getCost();
            m_sourcePlayer->delCoins(m_cardCoins);
            card->setState(State::Opening);
            return;
        }
    //如果是商店卡
    for(CardStore* cardStore:state->getCardStores())
        for(QList<Card*> slot:cardStore->getSlots())
            if(slot.last()->getId()==cardId){
                Card* card=slot.last();
                m_cardName=card->getName();
                m_cardCoins=card->getCost();
                m_sourcePlayer->delCoins(m_cardCoins);
                cardStore->delCard(card);
                m_sourcePlayer->addCard(card);
                return;
            }
    //报错
};

QString BuyCardCommand::getLog() const {
    QString log=QString(" %1 ").arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+="本轮没有建设任何建筑。";
    else
        log+=QString("建设了【%1】，花费了 %2 金币。").arg(m_cardName).arg(m_cardCoins);

    return log;
};

