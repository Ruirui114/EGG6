// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EGG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sound")
	float Sound = 0.5f;

	// 卵のスコア
	UPROPERTY(BlueprintReadWrite)
	int32 EggScore = 0;

	// スコア加算
	UFUNCTION(BlueprintCallable)
	void AddEggScore()
	{
		EggScore = 0;
	}

	// リセット（新規ゲーム用）
	UFUNCTION(BlueprintCallable)
	void ResetEggScore()
	{
		EggScore = 0;
	}
};
