#include "buycardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "cardstore.h"
#include"ai/ai.h"

BuyCardCommand::BuyCardCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::BuyCard, sourcePlayer,parent,cards,activePlayer){
}

//买综合收益最大的卡
PromptData BuyCardCommand::getPromptData(GameState* state) {
    int coins=m_sourcePlayer->getCoins();

    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要购买的牌，或建设一座地标建筑");
        //查找地表建筑
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark&&card->getState()==State::Closing){
                if(coins>=card->getCost())
                    pt.options.append(OptionData{card->getId(),card->getName(),1,""});
                else
                    pt.options.append(OptionData{card->getId(),card->getName(),0,"你没有足够的金币建设它。"});
            }
        }
        //查找商店建筑
        for(CardStore* store:state->getCardStores())
            for(Card* card:store->getCardFirst())
                if(card)
                    if(coins>=card->getCost()){
                        if(card->getColor()==Color::Purple&&m_sourcePlayer->getCardNum(card->getName(),State::None)>0)
                            pt.options.append(OptionData{card->getId(),card->getName(),0,"同名称的紫卡只能拥有一张。"});
                        else
                            pt.options.append(OptionData{card->getId(),card->getName(),1,""});
                    }
                    else
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"你没有足够的金币建设它。"});

        //不买按钮
        if(m_sourcePlayer->getCardNum("机场",State::Opening))//开了机场，显示不建设能+10元
            pt.options.append(OptionData{0,"不建设，获得10金币",1,""});
        else
            pt.options.append(OptionData{0,"不建设",1,""});
        return pt;
    }

    case 2:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要购买%1吗？").arg(state->getCard(m_userInput[0])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int BuyCardCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择收益最高的牌（很复杂的算法，这次先用买最贵的代替）
        AI* ai=state->getAI();
        ai->update(state);
        int opId=ai->getBuyCardId(promptData,m_sourcePlayer,state);
        return opId;
    }
    case 2:{//确认阶段
        return 1;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool BuyCardCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        //如果不买卡
        if(optionId==0){
            m_userInput.append(0);
            return true;
        }

        //如果买卡
        m_userInput.append(optionId);
        m_currentStep=2;
        return false;
    }
    case 2:{//确认阶段
        //确认则执行完毕
        if(optionId==1)
            return true;

        //否则重新选择
        m_userInput.clear();
        m_currentStep=1;
        return false;
    }
    }
    return true;
};


void BuyCardCommand::execute(GameState* state, GameController* controller){
    //读取选项
    int cardId=m_userInput[0];
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
        if(cards.last()->getId()==cardId){
            Card* card=cards.last();
            m_cardName=card->getName();
            m_cardCoins=card->getCost();
            m_sourcePlayer->delCoins(m_cardCoins);
            card->setState(State::Opening);
            //购物中心需要买的时候将所有相关建筑+1
            if(card->getName()=="购物中心")
                for(QList<Card*>cards2:m_sourcePlayer->getCards()){
                    for(Card* card2:cards2)
                        if(card2->getType()==Type::Store||card2->getType()==Type::Restaurant)
                            card2->changeValue(1);
                }
            return;
        }
    //如果是商店卡
    for(CardStore* cardStore:state->getCardStores())
        for(QList<Card*> slot:cardStore->getSlots())
            if(!slot.empty()&&slot.last()->getId()==cardId){
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

