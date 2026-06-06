#include "GameMode/NumberBaseballGameModeBase.h"

#include "Controller/NumberBaseballPlayerController.h"


ANumberBaseballGameModeBase::ANumberBaseballGameModeBase()
    : SecretNumber(""), NumberCount(3)
{
}

void ANumberBaseballGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    GenerateSecretNumber();

    UE_LOG(LogTemp, Warning, TEXT("Secret Number : %s") , *SecretNumber);
}

void ANumberBaseballGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ANumberBaseballPlayerController* PC = Cast<ANumberBaseballPlayerController>(NewPlayer);
    if (IsValid(PC))
    {
        PlayerControllers.Add(PC);
        SendMessageAllPlayers(FString::Printf(TEXT("%s has joined!!"), *NewPlayer->GetName()));
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

FString ANumberBaseballGameModeBase::JudgeResult(const FString& GuessString)
{
    int32 StrikeCount{}, BallCount{};

    FString Text = "";

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
        Text = "OUT";
    }
    else
    {
        Text = FString::Printf(TEXT("%dS %dB"), StrikeCount, BallCount);
    }

    return Text;
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

void ANumberBaseballGameModeBase::PrintChatMessage(ANumberBaseballPlayerController* PlayerController,
    const FString& Message)
{
    if (!IsValid(PlayerController)) return;

    if (IsGuessNumberString(Message))
    {
        FString Result = FString::Printf(TEXT("%s : "), *PlayerController->GetName()) + JudgeResult(Message);
        SendMessageAllPlayers(Result);
    }
    else
    {
        // 숫자 추측이 아니라면 채팅메세지로 보고 전달
        SendMessageAllPlayers(Message);
    }
}
