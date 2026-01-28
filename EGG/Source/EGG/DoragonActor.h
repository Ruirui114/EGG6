// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "DoragonActor.generated.h"
class USkeletalMeshComponent;
class UAnimSequence;
class UMaterialInterface;

UCLASS()
class EGG_API ADoragonActor : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	ADoragonActor();
	void SetRewardByScore(int32 Score);

protected:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Reward1;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Reward2;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Reward3;



	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimSequence* RewardUnderAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimSequence* RewardUperAnim;


	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* RewardMaterial1;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* RewardMaterial2;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* RewardMaterial3;
};
