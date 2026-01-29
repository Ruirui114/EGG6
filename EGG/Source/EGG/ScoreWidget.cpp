// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateEggCount();
}

int UScoreWidget::UpdateEggCount()
{
	//if (!EggText) return;

	//UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %p"), this);
	//UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %d"), Current);
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		FString Text = FString::Printf(TEXT("%d / 3"), GI->EggScore);
		EggText->SetText(FText::AsNumber(GI->EggScore));
	}
	return 0;
}
