#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumberBaseballInput.generated.h"


class UEditableTextBox;

UCLASS()
class LDH_09_API UNumberBaseballInput : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

protected:
    UFUNCTION()
    void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableTextBox> ChatInputTextBox;

private:
    // 채팅 텍스트 박스로 포커스 맞추는 함수
    void SetFocusChatInput();
};

