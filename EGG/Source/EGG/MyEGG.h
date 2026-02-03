// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ProgressBar.h"
#include "MyEGG.generated.h"


class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UTextBlock;
class UProgressBar;
class UMyWidget;
class ADoragonActor;

UCLASS()
class EGG_API AMyEgg : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyEgg();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** BallをControlする */
	void ControlBall(const FInputActionValue& Value);

	/** 視点を操作する */
	void Look(const FInputActionValue& Value);

public:
	void OnGoalReached();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetCheckpoint(const FVector& NewLocation);

	void ResumeFromMenu();

	void AddEggCount();
	// リスポーン待機時間（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float RespawnDelay = 2.0f;

	//ウィジット
	UPROPERTY()
	UUserWidget* GameOverWidgetInstance = nullptr;

	UPROPERTY()
	UUserWidget* ClearWidgetInstance = nullptr;

	UPROPERTY()
	UUserWidget* MenuWidgetInstance = nullptr;

	UPROPERTY()
	class UScoreWidget* ScoreWidget;

	// Boost
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxBoost = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentBoost = MaxBoost;

	//1秒に使うエネルギー
	UPROPERTY(EditAnywhere)
	float BoostConsumeRate = 20.0f;

	//1秒で回復する量
	UPROPERTY(EditAnywhere)
	float BoostRecoverRate = 20.0f;

	// --- Super Jump 用 ---
	UPROPERTY(EditAnywhere, Category = "SuperJump")
	float SuperJumpCostRatio = 0.5f;   // ゲージ50%

	UPROPERTY(EditAnywhere, Category = "SuperJump")
	float SuperJumpForce = 100000.0f;  // 上方向の力（調整用）

	// ウィジットクラス
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMyWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMyWidget> BoostWidgetClass;

	UMyWidget* BoostWidget;

	// クリアしたときに出すドラゴン
	UPROPERTY(EditAnywhere, Category = "Clear")
	ADoragonActor* DoragonActor;
private:
	//コンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Player")
	UStaticMesh* PlayerMesh;

	UPROPERTY(EditAnywhere, Category = "Physics")
	UPhysicalMaterial* PhysicsMaterial;

	// カメラ
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	//入力
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ControlAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* BoostAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SuperBoostAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* TabAction;

	// 地面判定用
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GroundCheckDistance = 50.0f; // 足元からのチェック距離

	// UI ウィジットクラス
	UPROPERTY(EditAnywhere,Category = "UI")
	TSubclassOf<UUserWidget> CooldownWidgetClass;

	UPROPERTY(EditAnywhere, Category = "GameOverUI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> BoostGaugeWidgetClass;

	UPROPERTY(EditAnywhere, Category = "ClearUI")
	TSubclassOf<UUserWidget> ClearWidgetClass;

	UPROPERTY(EditAnywhere, Category = "MenuUI")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ScoreWidgetClass;

	// ウィジットインスタンス
	UPROPERTY()
	UMyWidget* MyWidgetInstance;

	UPROPERTY()
	UUserWidget* BoostGaugeWidget;

	// Boostゲージ
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BoostBar;

	UPROPERTY()
	bool bIsGameOver = false;

	UPROPERTY()
	UUserWidget* CooldownWidget = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText = nullptr;

	/** Boost用フラグ */
	bool bCanBoost = true;

	/** Niagara エフェクト */
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* BoostEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* SuperBoostEffect;

	/** 再生中の Niagara Component */
	UPROPERTY()
	UNiagaraComponent* ActiveBoostEffect = nullptr;

	UPROPERTY()
	UNiagaraComponent* ActiveSuperBoostEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Boost|Sound")
	USoundBase* BoostSound;

	UPROPERTY()
	UAudioComponent* BoostAudioComponent;

	UPROPERTY(EditAnywhere, Category = "SuperBoost|Sound")
	USoundBase* SuperBoostSound;

	UPROPERTY()
	UAudioComponent* SuperBoostAudioComponent;

	UPROPERTY()
	float LandingHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallDistance = 800.0f;   // 300落ちたらリスタート

	UPROPERTY(VisibleAnywhere, Category = "Egg")
	int32 EggCount = 0;

	UPROPERTY(EditAnywhere, Category = "Egg")
	int32 MaxEggCount = 3;
	
	// リスポーン処理
	UFUNCTION()
	void RespawnPlayer();
	// Boost 処理
	UFUNCTION()
	void BoostStart(const FInputActionValue& Value);
	// Boost 停止処理
	UFUNCTION()
	void BoostStop(const FInputActionValue& Value);
	// SuperBoost
	UFUNCTION()
	void SuperJump();
	// メニュー
	UFUNCTION()
	void Tab();

	FVector RespawnPoint;
	float StartBoostHeight = 0.0f; // Boost した時の高さ

	// Boost ゲージ最大値
	UPROPERTY(EditAnywhere, Category = "Boost")
	float MaxBoostGauge = 100.0f;

	// 現在の Boost ゲージ
	UPROPERTY(VisibleAnywhere, Category = "Boost")
	float CurrentBoostGauge = 100.0f;

	// 加速力
	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostForcePower = 300000.0f;


	FVector BoostOffset = FVector(0, 0, 80); // プレイヤーの上に表示

	/** タイマー */
	FTimerHandle BoostTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;
	FTimerHandle CooldownTickTimer;

	/** ゴール関連 */
	bool bIsGoalReached = false;

	// Boost中に上昇中かどうか
	bool bIsRising = false;
	bool bIsBoostOnCooldown = false;
	bool bIsBoosting = false; // クールダウン中

	FVector2D LastMoveInput;

	bool bShouldRotateInAir = false;
	// 落下判定
	bool bShouldRespawnAfterLanding = false;

	// 地面判定用
	bool bIsGrounded = false;
	float FallDis = 0.0f;
	/** 各種設定値 */
	float Speed = 300.0f;
	float Health = 100.0f;
	float ForcePower = 150000.0f;
	// 上昇スピード
	float BoostRiseSpeed = 400.f; // 1秒間に30cm上がる
	/** 空中での操作の強さ（0.0〜1.0） */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float AirControlFactor = 0.7f;

	// 落下開始時の高さ
	float FallStartZ = 0.0f;

	// 落下中かどうか
	bool bIsFalling = false;

	APawn* Player;


};
