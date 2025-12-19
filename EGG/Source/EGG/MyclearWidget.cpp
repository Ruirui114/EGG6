// Fill out your copyright notice in the Description page of Project Settings.


#include "MyclearWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void  UMyclearWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TitleButton)
    {
        TitleButton->OnClicked.AddDynamic(this, &UMyclearWidget::OnTitleClicked);
    }
}

void UMyclearWidget::OnTitleClicked()
{
    // ゲーム開始（例：MainMap をロード）
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameTitle"));
}