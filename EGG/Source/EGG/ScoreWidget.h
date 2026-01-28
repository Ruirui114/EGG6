// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class EGG_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Boostゲージ更新用
	void UpdateBoostBar(float Current, float Max);
	// 卵カウント更新用
	UFUNCTION(BlueprintCallable)
	void UpdateEggCount(float Current, float Max);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EggText;
};
