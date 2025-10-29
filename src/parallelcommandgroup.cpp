#include "parallelcommandgroup.h"
#include <QDebug>

ParallelCommandGroup::ParallelCommandGroup(QObject* parent)
    : QObject(parent) {}

void ParallelCommandGroup::addCommand(GameCommand* cmd) {
    if (cmd) m_commands.append(cmd);
}

void ParallelCommandGroup::start(GameState* state, GameController* controller) {
    if (m_commands.isEmpty()) return;

    m_state = state;
    m_controller = controller;
    m_phase = 1;
    m_currentIndex = 0;
    m_finishedCount = 0;

    QTimer::singleShot(0, this, &ParallelCommandGroup::executeNext);
}

void ParallelCommandGroup::executeNext() {
    if (m_currentIndex >= m_commands.size()) {
        // 一轮执行完毕
        if (m_phase == 1) {
            // 开始第二轮
            m_phase = 2;
            m_currentIndex = 0;
            QTimer::singleShot(300, this, &ParallelCommandGroup::executeNextPhase);
        } else {
            // 第二轮结束 -> 所有命令结束
            emit finished(m_commands.first());
        }
        return;
    }

    GameCommand* cmd = m_commands[m_currentIndex];
    if (cmd) {
        qDebug() << QString("ParallelGroup phase %1: executing %2")
        .arg(m_phase)
            .arg(cmd->metaObject()->className());
        bool isFinish = cmd->setInput(1, m_state, m_controller);
        if (isFinish) {
            ++m_finishedCount;
        }
    }

    ++m_currentIndex;
    QTimer::singleShot(300, this, &ParallelCommandGroup::executeNext);
}

void ParallelCommandGroup::executeNextPhase() {
    executeNext();
}
