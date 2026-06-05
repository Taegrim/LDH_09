#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatPlayerController.generated.h"


class UChatInput;

UCLASS()
class LDH_09_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable)
    void PrintChatMessage(const FString& Message);

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UChatInput> ChatInputWidgetClass;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UChatInput> ChatInputWidgetInstance;

    FString ChatMessage;
};
