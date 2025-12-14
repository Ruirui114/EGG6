// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCheckpointo.generated.h"

UCLASS()
class EGG_API AMyCheckpointo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCheckpointo();

protected:
	// コリジョン用のボックス
	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	class UBoxComponent* TriggerBox;

	virtual void BeginPlay() override;
	// チェックポイントの位置
	FVector SavedLocation;

	// オーバーラップイベント
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	// 保存されるリスポーン位置
	FVector RespawnLocation;

};
