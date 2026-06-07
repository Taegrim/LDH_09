#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NumberBaseballGameStateBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemainingTimeChangedDelegate, int32, RemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentTurnChangedDelegate, FString, PlayerName);

UCLASS()
class LDH_09_API ANumberBaseballGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
    ANumberBaseballGameStateBase();

    UFUNCTION(BlueprintCallable)
    void ResetTimer();

    void DecreaseRemainTime();

    bool IsTimeOver() const;

    void SetCurrentTurnPlayer(FString NewTurnPlayerName);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    FString GetCurrentTurnPlayerName() const;

    UFUNCTION()
    void OnRep_RemainingTime();

    UFUNCTION()
    void OnRep_ChangedPlayerTurn();

public:
    UPROPERTY(BlueprintAssignable)
    FRemainingTimeChangedDelegate OnRemainingTimeChanged;

    UPROPERTY(BlueprintAssignable)
    FCurrentTurnChangedDelegate OnCurrentTurnChanged;

private:
    UPROPERTY(ReplicatedUsing = OnRep_RemainingTime)
    int32 RemainingTime;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    int32 MaxRemainingTime;

    UPROPERTY(ReplicatedUsing = OnRep_ChangedPlayerTurn)
    FString CurrentTurnPlayerName;
};
