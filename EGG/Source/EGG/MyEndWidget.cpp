// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEndWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void  UMyEndWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (EndButton)
    {
        EndButton->OnClicked.AddDynamic(this, &UMyEndWidget::OnEndClicked);
    }
}

void UMyEndWidget::OnEndClicked()
{
    // ゲーム開始（例：MainMap をロード）
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameTitle"));
}