// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCheckpointo.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyEGG.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMyCheckpointo::AMyCheckpointo()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetBoxExtent(FVector(200, 200, 200)); // ← チェックポイントの大きさ

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Overlap);

    // オーバーラップ登録
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMyCheckpointo::OnOverlapBegin);

}


void AMyCheckpointo::BeginPlay()
{
    Super::BeginPlay();

    // このチェックポイントの位置を保存しておく
    RespawnLocation = GetActorLocation();
}

void AMyCheckpointo::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)

{
    AMyEgg* Player = Cast<AMyEgg>(OtherActor);
    if (Player == nullptr) return;

    Player->SetCheckpoint(RespawnLocation);

    UE_LOG(LogTemp, Warning, TEXT("Checkpoint Updated: %s"), *RespawnLocation.ToString());


}