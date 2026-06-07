#include "PlayerState/NumberBaseballPlayerState.h"

#include "Net/UnrealNetwork.h"


ANumberBaseballPlayerState::ANumberBaseballPlayerState()
    : MaxTryCount(3)
{
    bReplicates = true;

    InitializeState();
}

void ANumberBaseballPlayerState::InitializeState()
{
    RemainTryCount = MaxTryCount;
}

void ANumberBaseballPlayerState::UseTryCount()
{
    int32 Count = FMath::Max(RemainTryCount - 1, 0);
    RemainTryCount = Count;
}

bool ANumberBaseballPlayerState::CanGuess() const
{
    return RemainTryCount > 0;
}

FString ANumberBaseballPlayerState::GetTryCountString() const
{
    return FString::Printf(TEXT(" (남은 시도 횟수 : %d)"), RemainTryCount);
}

void ANumberBaseballPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, RemainTryCount);
}
