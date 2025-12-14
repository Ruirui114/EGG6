// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWidget.h"
#include "Components/ProgressBar.h"

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