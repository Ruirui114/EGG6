// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMyWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMyWidget::UpdateBoostBar(float Current, float Max)
{
	if (!BoostBar) return;

	float Ratio = Current / Max;
	BoostBar->SetPercent(Ratio);
}

void UMyWidget::UpdateEggCount(float Current, float Max)
{
	//if (!EggText)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %p"), this);
	//	return;
	//}
	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("UpdateEggCount: %f"), Current);
	FString Text = FString::Printf(TEXT("%f / %f"), Current, Max);
	EggText->SetText(FText::FromString(Text));
}