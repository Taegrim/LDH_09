#include "Controller/NumberBaseballPlayerController.h"

#include "GameMode/NumberBaseballGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/NumberBaseballInput.h"


void ANumberBaseballPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!IsLocalController()) return;

    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    if (IsValid(ChatInputWidgetClass))
    {
        ChatInputWidgetInstance = CreateWidget<UNumberBaseballInput>(this, ChatInputWidgetClass);
        if (IsValid(ChatInputWidgetInstance))
        {
            ChatInputWidgetInstance->AddToViewport();
        }
    }
}

void ANumberBaseballPlayerController::SetChatMessage(const FString& Message)
{
    ChatMessage = Message;

    ServerPrintChatMessage(Message);
}

void ANumberBaseballPlayerController::PrintChatMessage(const FString& Message)
{
    UKismetSystemLibrary::PrintString(
        this,
        Message,
        true,
        true,
        FColor::Cyan,
        10.f
        );
}

void ANumberBaseballPlayerController::ServerPrintChatMessage_Implementation(const FString& Message)
{
    UWorld* World = GetWorld();
    if (!World) return;

    ANumberBaseballGameModeBase* GM = World->GetAuthGameMode<ANumberBaseballGameModeBase>();
    if (!GM) return;

    GM->PrintChatMessage(this, Message);
}

void ANumberBaseballPlayerController::ClientPrintChatMessage_Implementation(const FString& Message)
{
    PrintChatMessage(Message);
}
