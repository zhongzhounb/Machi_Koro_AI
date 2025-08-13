#include "buycardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "cardstore.h"
#include"ai/ai.h"
#include "commandfactory.h"

BuyCardCommand::BuyCardCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::BuyCard, sourcePlayer,parent,cards,activePlayer){
}

//买综合收益最大的卡
PromptData BuyCardCommand::getPromptData(GameState* state) const{
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
        //设置默认选项
        pt.autoInput=state->getAI()->getBuyCardId(pt,m_sourcePlayer,state);
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

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool BuyCardCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        m_userInput.append(optionId);

        //如果不买卡
        if(optionId==0)
            return true;

        //如果买卡
        m_currentStep=2;
        return false;
    }
    case 2:{//确认阶段
        //确认则执行完毕
        if(optionId==1){
            execute(state,controller);
            return true;
        }

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
        state->addLog(QString("%1本轮没有建设任何建筑。").arg(m_sourcePlayer->getName()));
        return ;
    }

    //买了的话得取消机场效果
    Card* card1=m_sourcePlayer->getCardsForName("机场").at(0);
    if(card1->getState()==State::Opening)
        controller->delCommand(controller->getCommands(card1->getSpecialType()).at(0));

    //创建命令，如果有科技公司
    QList<Card*> cards=m_sourcePlayer->getCardsForName("科技公司");
    if(cards.last()->getState()==State::Opening)
        controller->addCommand(CommandFactory::instance().createCommand(cards.last()->getSpecialType(),m_sourcePlayer,controller,cards,m_sourcePlayer));

    QString log=m_sourcePlayer->getName();
    //如果是建地标建筑
    for(QList<Card*>cards:m_sourcePlayer->getCards())
        if(cards.last()->getId()==cardId){
            Card* card=cards.last();
            log=QString("建设了【%1】，花费了 %2 金币。").arg(card->getName()).arg(card->getCost());
            m_sourcePlayer->delCoins(card->getCost());
            card->setState(State::Opening);
            //购物中心需要买的时候将所有相关建筑+1
            if(card->getName()=="购物中心")
                for(QList<Card*>cards2:m_sourcePlayer->getCards()){
                    for(Card* card2:cards2)
                        if(card2->getType()==Type::Store||card2->getType()==Type::Restaurant)
                            card2->changeValue(1);
                }
            break;
        }
    //如果是商店卡
    for(CardStore* cardStore:state->getCardStores())
        for(QList<Card*> slot:cardStore->getSlots())
            if(!slot.empty()&&slot.last()->getId()==cardId){
                Card* card=slot.last();
                log=QString("建设了【%1】，花费了 %2 金币。").arg(card->getName()).arg(card->getCost());
                m_sourcePlayer->delCoins(card->getCost());
                cardStore->delCard(card);
                m_sourcePlayer->addCard(card);
                break;
            }
    state->addLog(log);
};

