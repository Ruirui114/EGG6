// Fill out your copyright notice in the Description page of Project Settings.


#include "DoragonActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/StaticMeshComponent.h"
#include "MyGameInstance.h"
// Sets default values
ADoragonActor::ADoragonActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Reward1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Reward1"));
	Reward2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Reward2"));
	Reward3 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Reward3"));

	Reward1->SetupAttachment(RootComponent);
	Reward2->SetupAttachment(RootComponent);
	Reward3->SetupAttachment(RootComponent);

	// Å‰‚Í”ñ•\Ž¦
	Reward1->SetVisibility(false);
	Reward2->SetVisibility(false);
	Reward3->SetVisibility(false);

	Reward1->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Reward2->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Reward3->SetAnimationMode(EAnimationMode::AnimationSingleNode);
}
void ADoragonActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("DoragonActor BeginPlay: %s"), *GetName());

	int32 Score = 0;

	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		Score = GI->EggScore;
	}

	SetRewardByScore(Score);
}
void ADoragonActor::SetRewardByScore(int32 Score)
{
	UE_LOG(LogTemp, Warning, TEXT("SetRewardByScore called! Score = %d"), Score);

	Reward1->SetVisibility(false);
	Reward2->SetVisibility(false);
	Reward3->SetVisibility(false);

	Reward1->SetHiddenInGame(true);
	Reward2->SetHiddenInGame(true);
	Reward3->SetHiddenInGame(true);

	if (Score >= 1 && Reward1)
	{
		Reward1->SetHiddenInGame(false);
		Reward1->SetVisibility(true, true);

		if (RewardMaterial1)
		{
			Reward1->SetMaterial(0, RewardMaterial1);
		}

		if (RewardUnderAnim)
		{
			Reward1->PlayAnimation(RewardUnderAnim, true);
		}
	}

	if (Score >= 2 && Reward2)
	{
		Reward2->SetHiddenInGame(false);
		Reward2->SetVisibility(true, true);

		if (RewardMaterial2)
		{
			Reward2->SetMaterial(0, RewardMaterial2);
		}

		if (RewardUperAnim)
		{
			Reward2->PlayAnimation(RewardUperAnim, true);
		}
	}

	if (Score >= 3 && Reward3)
	{
		Reward3->SetHiddenInGame(false);
		Reward3->SetVisibility(true, true);

		if (RewardMaterial3)
		{
			Reward3->SetMaterial(0, RewardMaterial3);
		}

		if (RewardUperAnim)
		{
			Reward3->PlayAnimation(RewardUperAnim, true);
		}
	}
}

