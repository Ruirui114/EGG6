// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMoveActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMyMoveActor::AMyMoveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Root
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // Box
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->SetupAttachment(Root);
    BoxComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // Mesh
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(Root);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void AMyMoveActor::BeginPlay()
{
	Super::BeginPlay();
    StartLocation = GetActorLocation();
}

// Called every frame
void AMyMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FVector Target = StartLocation + (bMovingForward ? MoveOffset : FVector::ZeroVector);
    FVector Current = GetActorLocation();

    FVector NewLocation = FMath::VInterpConstantTo(
        Current,
        Target,
        DeltaTime,
        MoveSpeed
    );

    SetActorLocation(NewLocation);

    if (FVector::Dist(NewLocation, Target) < 1.f)
    {
        bMovingForward = !bMovingForward;
    }
}

