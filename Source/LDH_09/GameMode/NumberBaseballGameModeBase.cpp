#include "GameMode/NumberBaseballGameModeBase.h"

#include "Controller/NumberBaseballPlayerController.h"
#include "GameState/NumberBaseballGameStateBase.h"
#include "PlayerState/NumberBaseballPlayerState.h"


ANumberBaseballGameModeBase::ANumberBaseballGameModeBase()
    : SecretNumber(""), NumberCount(3), PlayerIndex(0), CurrentTurnIndex(INDEX_NONE), bRoundStarted(false)

{
}

// 게임 초기화
void ANumberBaseballGameModeBase::InitializeGame()
{
    GenerateSecretNumber();

    UE_LOG(LogTemp, Warning, TEXT("Secret Number : %s") , *SecretNumber);

    CurrentTurnIndex = INDEX_NONE;

    for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
    {
        if (!IsValid(PC)) continue;

        PC->ClientSetNotificationText(FText::FromString(TEXT("게임을 시작합니다!")));
    }

    HandleTurn();

    bRoundStarted = true;
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

    // 맨 처음 플레이어가 들어오면 그 때 시작하도록
    if (CurrentTurnIndex == INDEX_NONE)
    {
        InitializeGame();
    }
}

void ANumberBaseballGameModeBase::Logout(AController* Exiting)
{
    if (ANumberBaseballPlayerController* PC = Cast<ANumberBaseballPlayerController>(Exiting))
    {
        int32 ExitIndex = PlayerControllers.IndexOfByKey(PC);
        ANumberBaseballGameStateBase* GS = GetGameState<ANumberBaseballGameStateBase>();
        bool bIsTurnedPlayer = false;

        if (ExitIndex != INDEX_NONE)
        {
            PlayerControllers.RemoveAt(ExitIndex);

            if (PlayerControllers.Num() == 0)
            {
                CurrentTurnIndex = INDEX_NONE;

                // 타이머 정리
                GetWorldTimerManager().ClearTimer(RemainingTimerHandle);

                // 게임 상태 갱신
                if (IsValid(GS))
                {
                    GS->SetCurrentTurnPlayer(TEXT("None"));
                }
            }
            else if (ExitIndex < CurrentTurnIndex)
            {
                // 현재 턴 Index보다 앞 플레이어가 나가면 한칸 당기기
                --CurrentTurnIndex;
            }
            else if (ExitIndex == CurrentTurnIndex)
            {
                // 현재 턴을 가진 플레이어가 나가면 유지, 모듈러 연산만
                CurrentTurnIndex = CurrentTurnIndex % PlayerControllers.Num();
                bIsTurnedPlayer = true;
            }
            else if (CurrentTurnIndex >= PlayerControllers.Num())
            {
                // 현재 턴을 가진 플레이어가 나가서 턴 인덱스가 배열 바깥으로 나가면
                CurrentTurnIndex = 0;
            }

            // 턴을 가진 플레이어가 나갔다면 새로 턴을 부여해야 함
            // HandleTurn 은 다음 플레이어부터 턴을 부여하므로 1을 빼고 호출해야 함
            if (bIsTurnedPlayer && PlayerControllers.Num() != 0)
            {
                CurrentTurnIndex = FMath::Max(CurrentTurnIndex - 1, -1);
                HandleTurn();
            }
        }
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
        return FString::Printf(TEXT("%s -> OUT"), *GuessString);
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
    for (const TObjectPtr<ANumberBaseballPlayerController> PC : PlayerControllers)
    {
        if (!IsValid(PC)) continue;

        // 플레이어 상태 초기화, 현재는 시도횟수만 초기화 함
        ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();
        if (IsValid(PS))
        {
            PS->InitializeState();
        }
    }

    InitializeGame();
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
                FString NotificationText = PS->GetPlayerName() + TEXT(" 승리!  잠시 후 게임을 재시작합니다.");

                PC->ClientSetNotificationText(FText::FromString(NotificationText));
            }
        }

        GetWorldTimerManager().ClearTimer(RemainingTimerHandle);
        GetWorldTimerManager().SetTimer(
            ResetTimerHandle,
            this,
            &ANumberBaseballGameModeBase::HandleResetGame,
            3.f,
            false
            );
        bRoundStarted = false;
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
                    PC->ClientSetNotificationText(FText::FromString(TEXT("무승부..  잠시 후 게임을 재시작합니다.")));
                }
            }

            GetWorldTimerManager().ClearTimer(RemainingTimerHandle);
            GetWorldTimerManager().SetTimer(
                ResetTimerHandle,
                this,
                &ANumberBaseballGameModeBase::HandleResetGame,
                3.f,
                false
                );
            bRoundStarted = false;
            return;
        }

        HandleTurn();
    }
}

void ANumberBaseballGameModeBase::DecreaseRemainingTime()
{
    ANumberBaseballGameStateBase* GS = GetGameState<ANumberBaseballGameStateBase>();
    if (!IsValid(GS)) return;

    GS->DecreaseRemainTime();

    // 제한 시간 초과되면 기회 감소, 게임 판정 시도
    if (GS->IsTimeOver())
    {
        if (!PlayerControllers.IsValidIndex(CurrentTurnIndex)) return;

        ANumberBaseballPlayerController* PC = PlayerControllers[CurrentTurnIndex];
        if (!IsValid(PC)) return;

        ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();
        if (!IsValid(PS)) return;

        PS->UseTryCount();

        PC->ClientPrintChatMessage(TEXT("시간 초과!") + PS->GetTryCountString());

        JudgeGame(PC, 0);
    }
}

void ANumberBaseballGameModeBase::HandleTurn()
{
    if (PlayerControllers.Num() == 0)
    {
        CurrentTurnIndex = INDEX_NONE;
        return;
    }

    int32 NextTurnIndex = CurrentTurnIndex;
    bool bFoundPlayer = false;

    for (int32 i = 0; i < PlayerControllers.Num(); ++i)
    {
        if (NextTurnIndex == INDEX_NONE)
        {
            NextTurnIndex = 0;
        }
        else
        {
            NextTurnIndex = (NextTurnIndex + 1) % PlayerControllers.Num();
        }

        ANumberBaseballPlayerController* PC = PlayerControllers[NextTurnIndex];
        if (!IsValid(PC)) continue;

        ANumberBaseballPlayerState* PS = PC->GetPlayerState<ANumberBaseballPlayerState>();

        // 다음 순서 사람이 시도횟수가 남아있다면 현재 인덱스에서 종료
        if (IsValid(PS) && PS->CanGuess())
        {
            CurrentTurnIndex = NextTurnIndex;

            ANumberBaseballGameStateBase* GS = GetGameState<ANumberBaseballGameStateBase>();
            if (IsValid(GS))
            {
                // GameState에 현재 턴을 가진 플레이어 이름과 Index 저장
                GS->SetCurrentTurnPlayer(PS->GetPlayerName());
            }

            bFoundPlayer = true;
            break;
        }
    }

    // 턴을 부여할 플레이어가 없다면 타이머 정리
    if (!bFoundPlayer)
    {
        GetWorldTimerManager().ClearTimer(RemainingTimerHandle);
        return;
    }

    // 남은 시간 되돌림
    ANumberBaseballGameStateBase* GS = GetGameState<ANumberBaseballGameStateBase>();
    if (IsValid(GS))
    {
        GS->ResetTimer();
    }

    // 타이머 남아있다면 제거
    if (GetWorldTimerManager().IsTimerActive(RemainingTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(RemainingTimerHandle);
    }

    // 남은 시간 타이머 설정
    GetWorldTimerManager().SetTimer(
        RemainingTimerHandle,
        this,
        &ANumberBaseballGameModeBase::DecreaseRemainingTime,
        1.f,
        true
        );
}

void ANumberBaseballGameModeBase::HandleResetGame()
{
    ResetGame();
}

void ANumberBaseballGameModeBase::PrintChatMessage(ANumberBaseballPlayerController* PlayerController,
                                                   const FString& Message)
{
    if (!IsValid(PlayerController)) return;

    ANumberBaseballPlayerState* PS = PlayerController->GetPlayerState<ANumberBaseballPlayerState>();
    if (!IsValid(PS)) return;

    FString NameString = PS->GetPlayerName() + TEXT(" : ");

    // 숫자 추측이라면
    if (IsGuessNumberString(Message))
    {
        // 게임중이 아니라면
        if (!bRoundStarted)
        {
            PlayerController->ClientPrintChatMessage(TEXT("새로운 게임을 기다리는 중입니다..."));
            return;
        }

        // 시도횟수가 남아있는지 확인
        if (PS->CanGuess())
        {
            ANumberBaseballGameStateBase* GS = GetGameState<ANumberBaseballGameStateBase>();
            if (!IsValid(GS)) return;

            // 턴 부여받은 플레이어만 시도할 수 있게
            if (GS->GetCurrentTurnPlayerName() == PS->GetPlayerName())
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
                // 차례가 아니라고 해당 플레이어에게만 메세지 전달
                PlayerController->ClientPrintChatMessage(TEXT("아직 차례가 아닙니다."));
            }
        }
        else
        {
            // 시도횟수가 남아있지 않다면 해당 플레이어에게만 메세지 전달
            PlayerController->ClientPrintChatMessage(TEXT("시도 횟수를 모두 소진하였습니다."));
        }
    }
    else
    {
        // 숫자 추측이 아니라면 채팅메세지로 보고 전달
        SendMessageAllPlayers(NameString + Message);
    }
}
