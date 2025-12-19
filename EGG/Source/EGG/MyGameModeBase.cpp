// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "MyGameModeBase.h"
#include "MyWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AMyGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // ポーズ解除
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    // 入力をゲーム専用に戻す
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);

    // マウス非表示
    PC->bShowMouseCursor = false;

    // 念のため入力を有効化
    PC->EnableInput(PC);
}
