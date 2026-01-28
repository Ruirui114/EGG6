// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"
#include "Components/TextBlock.h"

void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("ScoreWidget NativeConstruct"));
}



void UScoreWidget::UpdateEggCount(float Current, float Max)
{
	//if (!EggText)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %p"), this);
	//	return;
	//}
	if (!EggText) return;

	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %f"), Current);
	FString Text = FString::Printf(TEXT("%.0f / %.0f"), Current, Max);
	EggText->SetText(FText::FromString(Text));
}
