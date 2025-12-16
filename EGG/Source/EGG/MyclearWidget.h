// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyclearWidget.generated.h"

class UButton;

UCLASS()
class EGG_API UMyclearWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* TitleButton;
private:
    UFUNCTION()
    void OnTitleClicked();
};
