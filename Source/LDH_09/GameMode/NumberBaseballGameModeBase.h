#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NumberBaseballGameModeBase.generated.h"


class ANumberBaseballPlayerController;

UCLASS()
class LDH_09_API ANumberBaseballGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
    ANumberBaseballGameModeBase();

    void PrintChatMessage(ANumberBaseballPlayerController* PlayerController, const FString& Message);

    void InitializeGame();

    virtual void PostLogin(APlayerController* NewPlayer) override;

    virtual void Logout(AController* Exiting) override;

private:
    // 숫자 개수만큼 비밀 숫자를 생성하는 함수
    void GenerateSecretNumber();

    // 제대로 숫자야구 추측 메세지가 왔는지 판단하는 함수
    bool IsGuessNumberString(const FString& Message);

    // 플레이어가 추측한 숫자와 비밀 숫자가 맞는지 판정하는 함수
    FString JudgeResult(const FString& GuessString, int32& StrikeCount, int32& BallCount);

    // 접속한 모든 플레이어에게 메세지를 전송하는 함수
    void SendMessageAllPlayers(const FString& Message);

    void ResetGame();

    void JudgeGame(APlayerController* ChatPlayerController, int32 StrikeCount);

    void DecreaseRemainingTime();

    void HandleTurn();

    void HandleResetGame();

private:
    FString SecretNumber;

    // 숫자 야구 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    int32 NumberCount;

    TArray<TObjectPtr<ANumberBaseballPlayerController>> PlayerControllers;

    // 로그인 시 부여할 플레이어 번호
    int32 PlayerIndex;

    FTimerHandle RemainingTimerHandle;

    // 현재 턴을 가진 플레이어 인덱스 번호
    int32 CurrentTurnIndex;

    FTimerHandle ResetTimerHandle;

    bool bRoundStarted;
};
