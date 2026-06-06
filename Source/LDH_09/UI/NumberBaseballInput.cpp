#include "UI/NumberBaseballInput.h"

#include "Components/EditableTextBox.h"
#include "Controller/NumberBaseballPlayerController.h"

void UNumberBaseballInput::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ChatInputTextBox->OnTextCommitted.IsAlreadyBound(this, &UNumberBaseballInput::OnChatInputTextCommitted))
    {
        ChatInputTextBox->OnTextCommitted.AddDynamic(this, &UNumberBaseballInput::OnChatInputTextCommitted);
    }

    SetFocusChatInput();
}

void UNumberBaseballInput::NativeDestruct()
{
    if (ChatInputTextBox->OnTextCommitted.IsAlreadyBound(this, &UNumberBaseballInput::OnChatInputTextCommitted))
    {
        ChatInputTextBox->OnTextCommitted.RemoveDynamic(this, &UNumberBaseballInput::OnChatInputTextCommitted);
    }

    Super::NativeDestruct();
}

void UNumberBaseballInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    // 엔터 입력 시
    if (CommitMethod == ETextCommit::OnEnter)
    {
        // 빈 입력을 보낼경우 키보드로 포커스만 주고 끝내기
        if (Text.IsEmpty())
        {
            SetFocusChatInput();
            return;
        }

        APlayerController* OwningPlayerController = GetOwningPlayer();
        if (!IsValid(OwningPlayerController)) return;

        ANumberBaseballPlayerController* ChatPlayerController = Cast<ANumberBaseballPlayerController>(OwningPlayerController);
        if (!IsValid(ChatPlayerController)) return;

        // 플레이어 컨트롤러에 채팅 메세지 저장하기
        ChatPlayerController->SetChatMessage(Text.ToString());

        // 채팅창 비워주기
        ChatInputTextBox->SetText(FText::GetEmpty());

        SetFocusChatInput();
    }
}

void UNumberBaseballInput::SetFocusChatInput()
{
    if (!IsValid(ChatInputTextBox)) return;

    APlayerController* PC = GetOwningPlayer();
    if (!IsValid(PC)) return;

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(ChatInputTextBox->TakeWidget());

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    ChatInputTextBox->SetKeyboardFocus();
}
