#include "Controller/ChatPlayerController.h"

#include "Kismet/KismetSystemLibrary.h"
#include "UI/ChatInput.h"


void AChatPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    if (IsValid(ChatInputWidgetClass))
    {
        ChatInputWidgetInstance = CreateWidget<UChatInput>(this, ChatInputWidgetClass);
        if (IsValid(ChatInputWidgetInstance))
        {
            ChatInputWidgetInstance->AddToViewport();
        }
    }
}

void AChatPlayerController::SetChatMessage(const FString& Message)
{
    ChatMessage = Message;

    PrintChatMessage(ChatMessage);
}

void AChatPlayerController::PrintChatMessage(const FString& Message)
{
    UKismetSystemLibrary::PrintString(
        this,
        Message,
        true,
        true,
        FColor::Cyan,
        5.f
        );
}
