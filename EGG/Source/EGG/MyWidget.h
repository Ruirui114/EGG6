// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyWidget.generated.h"

class UProgressBar;
UCLASS()
class EGG_API UMyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Boostゲージ更新用
	void UpdateBoostBar(float Current, float Max);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BoostBar;
};
