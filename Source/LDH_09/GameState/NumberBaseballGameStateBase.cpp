#include "GameState/NumberBaseballGameStateBase.h"

#include "Controller/NumberBaseballPlayerController.h"
#include "Net/UnrealNetwork.h"

ANumberBaseballGameStateBase::ANumberBaseballGameStateBase()
    : MaxRemainingTime(30)
{
    bReplicates = true;

    ResetTimer();
}

void ANumberBaseballGameStateBase::ResetTimer()
{
    RemainingTime = MaxRemainingTime;
}

void ANumberBaseballGameStateBase::DecreaseRemainTime()
{
    RemainingTime = FMath::Max(RemainingTime - 1, 0);
}

bool ANumberBaseballGameStateBase::IsTimeOver() const
{
    return RemainingTime <= 0;
}

void ANumberBaseballGameStateBase::SetCurrentTurnPlayer(FString NewTurnPlayerName)
{
    CurrentTurnPlayerName = NewTurnPlayerName;
}

void ANumberBaseballGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, RemainingTime);
    DOREPLIFETIME(ThisClass, CurrentTurnPlayerName);
}

FString ANumberBaseballGameStateBase::GetCurrentTurnPlayerName() const
{
    return CurrentTurnPlayerName;
}

void ANumberBaseballGameStateBase::OnRep_RemainingTime()
{
    OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void ANumberBaseballGameStateBase::OnRep_ChangedPlayerTurn()
{
    OnCurrentTurnChanged.Broadcast(CurrentTurnPlayerName);
}
