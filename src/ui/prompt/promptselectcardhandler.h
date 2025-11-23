#ifndef PROMPT_SELECT_CARD_HANDLER_H
#define PROMPT_SELECT_CARD_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QSharedPointer>
#include <QPointer>
#include <QMap>
#include <QList>

class CardWidget;
class SlotWidget;
class PlayerAreaWidget;
class CardStoreArea;

class PromptSelectCardHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptSelectCardHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
