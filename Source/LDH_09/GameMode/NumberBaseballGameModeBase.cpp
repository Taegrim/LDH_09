#include "GameMode/NumberBaseballGameModeBase.h"

#include "Controller/NumberBaseballPlayerController.h"
#include "PlayerState/NumberBaseballPlayerState.h"


ANumberBaseballGameModeBase::ANumberBaseballGameModeBase()
    : SecretNumber(""), NumberCount(3), PlayerIndex(0)
{
}

void ANumberBaseballGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    InitializeGame();
}


void ANumberBaseballGameModeBase::InitializeGame()
{
    GenerateSecretNumber();

    UE_LOG(LogTemp, Warning, TEXT("Secret Number : %s") , *SecretNumber);
}

void ANumberBaseballGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ANumberBaseballPlayerController* PC = Cast<ANumberBaseballPlayerController>(NewPlayer);
    if (IsValid(PC))
    {
        PC->ClientSetNotificationText(FText::FromString(TEXT("서버에 접속했습니다...")));
        PlayerControllers.Add(PC);

        ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();
        if (IsValid(PS))
        {
            PS->SetPlayerName(FString::Printf(TEXT("Player %d"), ++PlayerIndex));

            SendMessageAllPlayers(FString::Printf(TEXT("%s has joined!!"), *PS->GetPlayerName()));
        }
    }
}

void ANumberBaseballGameModeBase::Logout(AController* Exiting)
{
    if (ANumberBaseballPlayerController* PC = Cast<ANumberBaseballPlayerController>(Exiting))
    {
        PlayerControllers.Remove(PC);
    }

    Super::Logout(Exiting);
}

void ANumberBaseballGameModeBase::GenerateSecretNumber()
{
    // 최대 9개까지
    NumberCount = FMath::Clamp(NumberCount, 1, 9);

    TArray<int32> Numbers { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // 난수시드 초기화
    FRandomStream RandomStream;
    RandomStream.GenerateNewSeed();

    FString Number;
    for (int32 i = 0; i < NumberCount; ++i)
    {
        int32 Index = RandomStream.RandRange(0, Numbers.Num() - 1);
        Number += FString::FromInt(Numbers[Index]);
        Numbers.RemoveAtSwap(Index);
    }
    SecretNumber = Number;
}

bool ANumberBaseballGameModeBase::IsGuessNumberString(const FString& Message)
{
    // 길이가 다르면 숫자 추측으로 판단하지 않음
    if (Message.Len() != NumberCount) return false;

    // 제대로 숫자 추측이 왔는지 검사, 숫자인지, 중복이 없는지 확인
    bool bIsNumber = true;
    TSet<TCHAR> UniqueNumbers;

    for (TCHAR C : Message)
    {
        if (!FChar::IsDigit(C) || C == TEXT('0'))
        {
            bIsNumber = false;
            break;
        }
        UniqueNumbers.Add(C);
    }

    // 중복 숫자가 있다면 숫자 개수와 다르므로 false 반한
    if (UniqueNumbers.Num() != NumberCount) return false;

    return bIsNumber;
}

FString ANumberBaseballGameModeBase::JudgeResult(const FString& GuessString, int32& StrikeCount, int32& BallCount)
{
    StrikeCount = 0;
    BallCount = 0;

    for (int32 i = 0; i < NumberCount; ++i)
    {
        if (SecretNumber[i] == GuessString[i])
        {
            ++StrikeCount;
        }
        else
        {
            FString Num = FString::Printf(TEXT("%c"), GuessString[i]);
            if (SecretNumber.Contains(Num))
            {
                ++BallCount;
            }
        }
    }

    if (StrikeCount == 0 && BallCount == 0)
    {
        return TEXT("OUT");
    }

    return FString::Printf(TEXT("%s -> %dS %dB"), *GuessString, StrikeCount, BallCount);
}

void ANumberBaseballGameModeBase::SendMessageAllPlayers(const FString& Message)
{
    for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
    {
        if (IsValid(PC))
        {
            PC->ClientPrintChatMessage(Message);
        }
    }
}

void ANumberBaseballGameModeBase::ResetGame()
{
    InitializeGame();

    for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
    {
        if (!IsValid(PC)) continue;

        ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();
        if (IsValid(PS))
        {
            PS->InitializeState();
        }
    }
}

void ANumberBaseballGameModeBase::JudgeGame(APlayerController* ChatPlayerController, int32 StrikeCount)
{
    // 모두 맞췄으면 게임 종료
    if (StrikeCount == NumberCount)
    {
        for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
        {
            if (!IsValid(PC)) continue;

            APlayerState* PS = ChatPlayerController->GetPlayerState<APlayerState>();
            if (IsValid(PS))
            {
                FString NotificationText = PS->GetPlayerName() + TEXT(" 승리!!");

                PC->ClientSetNotificationText(FText::FromString(NotificationText));
            }
        }

        ResetGame();
    }
    else
    {
        bool bIsDraw = true;

        for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
        {
            if (!IsValid(PC)) continue;

            ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();
            if (IsValid(PS))
            {
                // 한명이라도 추측 횟수 남아있다면 무승부 아님
                if (PS->CanGuess())
                {
                    bIsDraw = false;
                }
            }
        }

        // 무승부라면 알림 보내고 게임 재시작
        if (bIsDraw)
        {
            for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
            {
                if (IsValid(PC))
                {
                    PC->ClientSetNotificationText(FText::FromString(TEXT("무승부...")));
                }
            }

            ResetGame();
        }
    }
}

void ANumberBaseballGameModeBase::PrintChatMessage(APlayerController* PlayerController,
                                                   const FString& Message)
{
    if (!IsValid(PlayerController)) return;

    ANumberBaseballPlayerState* PS = PlayerController->GetPlayerState<ANumberBaseballPlayerState>();
    if (!IsValid(PS)) return;

    FString NameString = PS->GetPlayerName() + TEXT(" : ");

    if (IsGuessNumberString(Message))
    {
        // 시도횟수가 남아있는지 확인
        if (PS->CanGuess())
        {
            PS->UseTryCount();

            int32 StrikeCount = 0;
            int32 BallCount = 0;

            // 결과에 이름, 추측 횟수 붙이기
            FString JudgeString = JudgeResult(Message, StrikeCount, BallCount);
            FString Result = NameString + JudgeString + PS->GetTryCountString();

            // 모든 플레이어에게 결과 보내기
            SendMessageAllPlayers(Result);

            // 게임 판정
            JudgeGame(PlayerController, StrikeCount);
        }
        else
        {
            // 시도횟수가 남아있지 않다면 해당 플레이어에게만 메세지 전달
            ANumberBaseballPlayerController* BaseballPC = Cast<ANumberBaseballPlayerController>(PlayerController);
            if (!IsValid(BaseballPC)) return;

            BaseballPC->ClientPrintChatMessage(TEXT("시도 횟수를 모두 소진하였습니다."));
        }
    }
    else
    {
        // 숫자 추측이 아니라면 채팅메세지로 보고 전달
        SendMessageAllPlayers(NameString + Message);
    }
}
