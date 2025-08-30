#include "gamecommand.h"
#include "gamestate.h" // 需要访问 GameState 来查找 Player*
#include "player.h"    // 需要 Player 类的定义
#include "card.h"
#include "gamecontroller.h"
#include "factorys/commandfactory.h"
// 初始化静态成员变量
int GameCommand::s_commandId=0;

GameCommand::GameCommand(CommandType type,Player* sourcePlayer, QObject* parent,
                         QList<Card*> cards,Player* activePlayer)
    : m_type(type),QObject(parent),  m_sourcePlayer(sourcePlayer),m_cards(cards),m_activePlayer(activePlayer){
    //自增id，用以回放
    m_id=s_commandId++;

    m_card = nullptr; // 显式初始化 m_card 为 nullptr
    if(!m_cards.empty()) {
        m_card=m_cards.last();
    }

    /*创建命令自动计算优先级*/
    //1.首先排命令优先级
    m_priority=type*10000;
    //2.命令相同则排相同名称卡
    // 修正：只有当 m_card 非空时才执行卡牌相关的优先级计算
    if(m_card){ // 检查 m_card 是否为有效指针
        m_priority+=m_card->getNameId()*100;
        //3.相同名称卡只有红卡有执行顺序，优先执行上家
        // 修正：在解引用 m_sourcePlayer 和 m_activePlayer 之前进行空指针检查
        if(m_card->getColor()==Color::Red && m_sourcePlayer != nullptr && m_activePlayer != nullptr) {
            m_priority+=(m_sourcePlayer->getId()-m_activePlayer->getId()+MAX_PLAYER_NUM)%MAX_PLAYER_NUM;
        }
    }

}

PromptData GameCommand::getPromptData(GameState* state) const{
    PromptData pd;//之后可能要补充卡牌动画
    return pd;
};

bool GameCommand::setInput(int optionId,GameState* state,GameController* controller){
    execute(state,controller);
    return true;
};

// 修正：在 delCommand 和 addCommand 中也需要对 m_sourcePlayer 进行空指针检查
void GameCommand::delCommand(QString name,GameController* controller){
    if (m_sourcePlayer) { // 添加空指针检查
        QList<Card*> cards1 = m_sourcePlayer->getCardsForName(name);
        if(!cards1.empty()){
            Card* card1 = cards1.at(0);
            if(card1 && card1->getState()==State::Opening)
                controller->delCommand(controller->getCommands(card1->getSpecialType()).at(0));
        }
    } else {
        qWarning() << "delCommand called with null m_sourcePlayer.";
    }
}

void GameCommand::addCommand(QString name,GameController* controller){
    if (m_sourcePlayer) { // 添加空指针检查
        QList<Card*> cards=m_sourcePlayer->getCardsForName(name);
        if(!cards.empty()&&cards.last()->getState()==State::Opening)
            controller->addCommand(CommandFactory::instance().createCommand(cards.last()->getSpecialType(),m_sourcePlayer,controller,cards,m_sourcePlayer));
    } else {
        qWarning() << "addCommand called with null m_sourcePlayer.";
    }
}
