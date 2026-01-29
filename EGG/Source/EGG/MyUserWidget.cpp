// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
void  UMyUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnStartClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnExitClicked);
    }
}

void UMyUserWidget::OnStartClicked()
{
    // ゲーム開始（例：MainMap をロード）
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("NewMap"));
}


void UMyUserWidget::OnExitClicked()
{
    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        nullptr,         // Player Controller（nullptrでOK）
        EQuitPreference::Quit,
        true             // bIgnorePlatformRestrictions（true推奨）
    );
}