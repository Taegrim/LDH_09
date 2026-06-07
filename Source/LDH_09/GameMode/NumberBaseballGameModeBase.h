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

    void PrintChatMessage(APlayerController* PlayerController, const FString& Message);

    void InitializeGame();

    virtual void BeginPlay() override;

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

private:
    FString SecretNumber;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    int32 NumberCount;

    TArray<TObjectPtr<ANumberBaseballPlayerController>> PlayerControllers;

    int32 PlayerIndex;
};
