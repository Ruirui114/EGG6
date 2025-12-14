// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class EGG_API ATitleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	void BeginPlay();
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> TitleWidgetClass;

	UUserWidget* TitleWidget;
};
