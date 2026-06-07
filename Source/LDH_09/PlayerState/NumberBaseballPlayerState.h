#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NumberBaseballPlayerState.generated.h"


UCLASS()
class LDH_09_API ANumberBaseballPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    ANumberBaseballPlayerState();

    UFUNCTION(BlueprintCallable)
    void InitializeState();

    void UseTryCount();

    bool CanGuess() const;

    FString GetTryCountString() const;

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
    int32 MaxTryCount;

    UPROPERTY(Replicated)
    int32 RemainTryCount;
};
