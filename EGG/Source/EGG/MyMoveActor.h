// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMoveActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class EGG_API AMyMoveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyMoveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

    // Root
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    // ìñÇΩÇËîªíË
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* BoxComponent;

    // å©ÇΩñ⁄
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* MeshComponent;

    // à⁄ìÆê›íËÅiBP Ç≈í≤êÆÅj
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    FVector MoveOffset = FVector(800.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    float MoveSpeed = 200.f;

private:
    FVector StartLocation;
    bool bMovingForward = true;
};
