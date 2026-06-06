#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NumberBaseballPlayerController.generated.h"


class UNumberBaseballInput;

UCLASS()
class LDH_09_API ANumberBaseballPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable)
    void PrintChatMessage(const FString& Message);

    UFUNCTION(Server, Reliable)
    void ServerPrintChatMessage(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientPrintChatMessage(const FString& Message);

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNumberBaseballInput> ChatInputWidgetClass;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UNumberBaseballInput> ChatInputWidgetInstance;

    FString ChatMessage;
};
