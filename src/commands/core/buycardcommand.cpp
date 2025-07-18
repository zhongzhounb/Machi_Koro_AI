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
                if(coins>=card->getCost()){
                    if(card->getColor()==Color::Purple&&m_sourcePlayer->getCardNum(card->getName(),State::None)>0)
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"同名称的紫卡只能拥有一张。"});
                    else
                        pt.options.append(OptionData{card->getId(),card->getName(),1,""});
                }
                else
                    pt.options.append(OptionData{card->getId(),card->getName(),0,"你没有足够的金币建设它。"});

        return pt;
    }
    case 2:{//选择玩家阶段
        pt.type=PromptData::SelectPlayer;
        pt.promptMessage=QString("请选择将%1赠予的玩家").arg(state->getCard(m_userInput[0])->getName());
        for(Player* player:state->getPlayers())
            if(player!=m_sourcePlayer)
                pt.options.append(OptionData{player->getId(),player->getName(),1,""});

        return pt;
    }
    case 3:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将%1给予给%2吗？").arg(state->getCard(m_userInput[0])->getName()).arg(state->getPlayer(m_userInput[1])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int GiveCardCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择卡阶段，默认选本卡
        return m_card->getId();
    }
    case 2:{//选择玩家阶段，随机选择玩家
        int opId=promptData.options[RandomUtils::instance().generateInt(0,promptData.options.size()-1)].id;
        m_randIndex.append(1);//保存随机数下标
        return opId;
    }
    case 3:{//选择玩家阶段
        return 1;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool GiveCardCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        m_userInput.append(optionId);
        m_currentStep=2;
        return false;
    }
    case 2:{//选择玩家阶段
        m_userInput.append(optionId);
        m_currentStep=3;
        return false;
    }
    case 3:{//选择玩家阶段
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

