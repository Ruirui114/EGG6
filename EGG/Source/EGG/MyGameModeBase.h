// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
class UScoreWidget;
UCLASS()
class EGG_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UScoreWidget> ScoreWidgetClass;

	UPROPERTY()
	UScoreWidget* ScoreWidgetInstance;
};
