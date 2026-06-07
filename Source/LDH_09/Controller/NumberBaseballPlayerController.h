#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NumberBaseballPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationTextChangedDelegate, FText, NotificationText);

class UNumberBaseballInput;
class UUserWidget;

UCLASS()
class LDH_09_API ANumberBaseballPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ANumberBaseballPlayerController();

    UFUNCTION(BlueprintCallable)
    void SetChatMessage(const FString& Message);

    UFUNCTION(BlueprintCallable)
    void PrintChatMessage(const FString& Message);

    UFUNCTION(Server, Reliable)
    void ServerPrintChatMessage(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientPrintChatMessage(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientSetNotificationText(const FText& Message);

    UFUNCTION(BlueprintCallable)
    void SetNotificationText(const FText& Message);

public:
    UPROPERTY(BlueprintAssignable)
    FNotificationTextChangedDelegate OnNotificationTextChanged;

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNumberBaseballInput> ChatInputWidgetClass;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UNumberBaseballInput> ChatInputWidgetInstance;

    FString ChatMessage;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> NotificationWidgetClass;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UUserWidget> NotificationWidgetInstance;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FText NotificationText;
};
