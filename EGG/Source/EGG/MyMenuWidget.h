// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MyGameInstance.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyMenuWidget.generated.h"

class UButton;
class USlider;
UCLASS()
class EGG_API UMyMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bar");
    //USlider* Slider;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* TitleButton;

    UPROPERTY(meta = (BindWidget))
    UButton* RestartButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResumeButton;

    UPROPERTY(meta = (BindWidget))
    USlider* VolumeSlider;

    UFUNCTION()
    void OnSliderValueChanged(float Value);
private:
    UFUNCTION()
    void OnTitleClicked();

    UFUNCTION()
    void OnRestartClicked();

    UFUNCTION()
    void OnResumeClicked();

    UMyGameInstance* GameInstance;
};
