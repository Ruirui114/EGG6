// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyMenuWidget.generated.h"

class UButton;

UCLASS()
class EGG_API UMyMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* TitleButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RestartButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResumeButton;
private:
    UFUNCTION()
    void OnTitleClicked();

    UFUNCTION()
    void OnRestartClicked();

    UFUNCTION()
    void OnResumeClicked();
};
