// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void ATitleGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Widget生成
    if (TitleWidgetClass)
    {
        TitleWidget = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
        if (TitleWidget)
        {
            TitleWidget->AddToViewport();
        }
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // TitleWidget が null だったらフォーカス設定しない
        FInputModeUIOnly InputMode;

        if (TitleWidget)
        {
            InputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
        }

        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}