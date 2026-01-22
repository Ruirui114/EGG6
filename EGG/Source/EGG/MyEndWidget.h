// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyEndWidget.generated.h"

class UButton;

UCLASS()
class EGG_API UMyEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* EndButton;
private:
    UFUNCTION()
    void OnEndClicked();
};
