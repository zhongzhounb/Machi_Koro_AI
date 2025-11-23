#include "PromptSelectPlayerHandler.h"
#include "MainWindow.h"

PromptSelectPlayerHandler::PromptSelectPlayerHandler(MainWindow* main)
    : PromptHandlerBase(main) {}

void PromptSelectPlayerHandler::handle(const PromptData& pd)
{
    int opId = pd.autoInput;

    QTimer::singleShot(500, m_main, [this, opId]() {
        if (m_main->m_animationOverlayWidget)
            m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        emit responseUserInput(opId);
    });
}
