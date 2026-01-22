// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
void AEndGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Widget生成
    if (EndWidgetClass)
    {
        EndWidget = CreateWidget<UUserWidget>(GetWorld(), EndWidgetClass);
        if (EndWidget)
        {
            EndWidget->AddToViewport();
        }
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // TitleWidget が null だったらフォーカス設定しない
        FInputModeUIOnly InputMode;

        if (EndWidget)
        {
            InputMode.SetWidgetToFocus(EndWidget->TakeWidget());
        }

        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}