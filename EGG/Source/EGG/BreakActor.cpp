#include "BreakActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "MyEGG.h"

ABreakActor::ABreakActor()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // BOX 当たり判定
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->SetupAttachment(RootComponent);
    BoxComponent->SetBoxExtent(FVector(30.f, 30.f, 5.f));
    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->SetGenerateOverlapEvents(true);

    // メッシュ（表示用）
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetGenerateOverlapEvents(false);
    MeshComponent->SetVisibility(true);
    MeshComponent->SetHiddenInGame(false);
}

void ABreakActor::BeginPlay()
{
    Super::BeginPlay();

    // オーバーラップ開始時のイベント登録
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakActor::OnOverlapBegin);

}

void ABreakActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsBroken) return;

    // プレイヤーが乗ったか確認
    if (OtherActor && OtherActor->IsA(AMyEgg::StaticClass()))
    {
        // 1秒後に壊す
        GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ABreakActor::BreakPlatform, 1.0f, false);
    }
}

void ABreakActor::BreakPlatform()
{
    if (bIsBroken) return;

    bIsBroken = true;

    //非表示・当たり判定OFFにする
    MeshComponent->SetVisibility(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 3秒後（RespawnDelay）に復活
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ABreakActor::RespawnPlatform, RespawnDelay, false);
}

void ABreakActor::RespawnPlatform()
{
    // 表示・当たり判定ONに戻す
    MeshComponent->SetVisibility(true);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    bIsBroken = false;
}