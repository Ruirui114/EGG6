// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyEGG.h"

void  UMyMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TitleButton)
    {
        TitleButton->OnClicked.AddDynamic(this, &UMyMenuWidget::OnTitleClicked);
    }


    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UMyMenuWidget::OnRestartClicked);
    }


    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UMyMenuWidget::OnResumeClicked);
    }
}

void UMyMenuWidget::OnTitleClicked()
{
    // ゲーム開始（例：MainMap をロード）
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameTitle"));
}

void UMyMenuWidget::OnRestartClicked()
{
    // ゲーム開始
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("NewMap"));

    // プレイヤーコントローラ取得
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 入力をゲームに戻す
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;

        // 入力再有効化（念のため）
        PC->EnableInput(PC);
    }
}

void UMyMenuWidget::OnResumeClicked()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;
    // ポーズ解除
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    if (PC)
    {
        // 入力をゲームに戻す
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
        PC->EnableInput(PC);

        if (APawn* Pawn = PC->GetPawn())
        {
            if (AMyEgg* Egg = Cast<AMyEgg>(Pawn))
            {
                Egg->SetMenuOpen(false);
                Egg->ResumeFromMenu();
            }
        }
    }

    // このメニューを消す
    RemoveFromParent();
}
