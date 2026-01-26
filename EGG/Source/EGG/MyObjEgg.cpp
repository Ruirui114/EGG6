// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObjEgg.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyEGG.h"

// Sets default values
AMyObjEgg::AMyObjEgg()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->SetSphereRadius(50.f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyObjEgg::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMyObjEgg::OnOverlap);
}

void AMyObjEgg::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32,
	bool,
	const FHitResult&
)
{
	if (AMyEgg* Player = Cast<AMyEgg>(OtherActor))
	{
		Player->AddEggCount();   // ← プレイヤーに通知
		Destroy();               // ← 卵消える
	}
}

