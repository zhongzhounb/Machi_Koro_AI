#ifndef PROMPT_CARD_OUT_ANIMATION_HANDLER_H
#define PROMPT_CARD_OUT_ANIMATION_HANDLER_H

#include "PromptHandlerBase.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPointer>
#include <QTimer>

class SlotWidget;
class Player;
struct PlayerLayoutConfig;

class PromptCardOutAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptCardOutAnimationHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
