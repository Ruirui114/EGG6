// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMenuWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyEGG.h"
#include "MyGameInstance.h"

void  UMyMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (VolumeSlider)
    {
        // Sliderイベント登録
        VolumeSlider->OnValueChanged.AddDynamic(
            this,
            &UMyMenuWidget::OnSliderValueChanged
        );

        // GameInstanceから値を取得して代入
        if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
        {
            VolumeSlider->SetValue(GI->Sound);
        }
    }

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

void UMyMenuWidget::OnSliderValueChanged(float Value)
{
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {
        GI->Sound = Value;
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

        UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
        GI->ResetEggScore();

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
                Egg->ResumeFromMenu();

            }
        }
    }

    // このメニューを消す
    RemoveFromParent();
}

