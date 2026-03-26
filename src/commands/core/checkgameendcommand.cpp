#include "checkgameendcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

CheckGameEndCommand::CheckGameEndCommand(Player* sourcePlayer, QObject* parent, QList<Card*> cards, Player* activePlayer)
    : GameCommand(CommandType::CheckGameEnd, sourcePlayer, parent, cards, activePlayer)
{
}

bool CheckGameEndCommand::isWinCondition(Player* player) const
{
    if (!player) {
        return false;
    }

    for (const QList<Card*>& cards : player->getCards()) {
        if (cards.isEmpty()) {
            continue;
        }
        Card* card = cards.last();
        if (card && card->getType() == Type::Landmark && card->getState() == State::Closing) {
            return false;
        }
    }
    return true;
}

PromptData CheckGameEndCommand::getPromptData(GameState* state) const
{
    Q_UNUSED(state);

    PromptData pd;
    m_isWin = isWinCondition(m_sourcePlayer);
    if (!m_isWin) {
        pd.type = PromptData::None;
        pd.isAutoInput = true;
        pd.autoInput = 1;
        return pd;
    }

    pd.type = PromptData::Popup;
    pd.isAutoInput = false;
    pd.promptMessage = QString("%1 胜利了！").arg(m_sourcePlayer ? m_sourcePlayer->getName() : "玩家");
    pd.options.append(OptionData{1, "返回", 1, ""});
    return pd;
}

bool CheckGameEndCommand::setInput(int optionId, GameState* state, GameController* controller)
{
    Q_UNUSED(optionId);

    execute(state, controller);
    return true;
}

void CheckGameEndCommand::execute(GameState* state, GameController* controller)
{
    Q_UNUSED(state);

    if (!m_isWin) {
        return;
    }

    if (m_sourcePlayer) {
        state->addLog(QString("游戏结束！玩家 %1 获胜！").arg(m_sourcePlayer->getName()));
    } else {
        state->addLog(QString("游戏结束！"));
    }

    if (controller) {
        controller->endGameAndReturnToMenu();
    }
}
