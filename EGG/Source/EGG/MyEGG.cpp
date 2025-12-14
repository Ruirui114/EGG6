// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEGG.h"

#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "MyCheckpointo.h"
#include "Engine/StaticMesh.h"
#include "Blueprint/UserWidget.h"
#include "MyWidget.h"
#include "MyGameModeBase.h"
#include "Components/TextBlock.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h" 
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AMyEgg::AMyEgg()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// メッシュコンポーネント作成
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// デフォルトのメッシュを読み込み（エディタで差し替え可能）
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
	}


	// 衝突設定
	MeshComp->SetCollisionProfileName(TEXT("Pawn"));


	// 移動コンポーネント追加（物理ではなくコード制御）
	AutoPossessPlayer = EAutoReceiveInput::Player0;




	// SpringArmを追加する
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	// Spring Armの長さを調整する
	SpringArm->TargetArmLength = 450.0f;

	// PawnのControllerRotationを使用する
	SpringArm->bUsePawnControlRotation = true;

	// CameraのLagを有効にする
	SpringArm->bEnableCameraLag = true;

	// Cameraを追加する
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	// MotionBlurをオフにする
	Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

	// Input Mapping Context「IM_Controls」を読み込む
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/PlayerInput"));

	// Input Action「IA_Control」を読み込む
	ControlAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Control"));

	// Input Action「IA_Look」を読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Look"));

	// Input Action「IA_Boost」を読み込む
	BoostAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Boost"));

	// デフォルト値
	bIsGoalReached = false;
}

// Called when the game starts or when spawned
void AMyEgg::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BoostAction = %s"),
		BoostAction ? TEXT("OK") : TEXT("NULL"));
	StartBoostHeight = GetActorLocation().Z;

	//
	MeshComp->SetMobility(EComponentMobility::Movable);
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor")); // 物理用プロファイル
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);

	if (CooldownWidgetClass)
	{
		CooldownWidget = CreateWidget<UUserWidget>(GetWorld(), CooldownWidgetClass);

		if (CooldownWidget)
		{
			CooldownWidget->AddToViewport();
			CooldownWidget->SetVisibility(ESlateVisibility::Hidden);
			CooldownText = Cast<UTextBlock>(CooldownWidget->GetWidgetFromName(TEXT("Cooldown")));
		}
	}

	if (BoostGaugeWidgetClass)
	{
		BoostGaugeWidget = CreateWidget<UUserWidget>(GetWorld(), BoostGaugeWidgetClass);
		if (BoostGaugeWidget)
		{
			BoostGaugeWidget->AddToViewport();
			BoostBar = Cast<UProgressBar>(BoostGaugeWidget->GetWidgetFromName(TEXT("BoostBar")));
		}

		if (BoostBar)
		{
			UE_LOG(LogTemp, Warning, TEXT("BoostBar = OK"));
		}
		else
			UE_LOG(LogTemp, Error, TEXT("BoostBar = NULL !!!"));
	}

	if (BoostSound)
	{
		BoostAudioComponent = NewObject<UAudioComponent>(this);
		BoostAudioComponent->SetupAttachment(RootComponent);
		BoostAudioComponent->SetSound(BoostSound);
		BoostAudioComponent->bAutoActivate = false; // 自動再生 OFF
		BoostAudioComponent->bIsUISound = false;
		BoostAudioComponent->RegisterComponent();
	}

	if (PlayerMesh)
	{
		MeshComp->SetStaticMesh(PlayerMesh);
	}

	if (MeshComp && PhysicsMaterial)
	{
		MeshComp->SetPhysMaterialOverride(PhysicsMaterial);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				if (DefaultMappingContext)
				{
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}


}

// Tick関数で位置だけを同期（回転は無視）
void AMyEgg::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGoalReached) return; // ← ゴール後は物理処理をスキップ
	float CurrentZ = GetActorLocation().Z;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		bool bBoosting = PC->IsInputKeyDown(EKeys::SpaceBar);

		if (bBoosting && CurrentBoost > 0.0f)
		{
			// Boost 消費
			CurrentBoost -= BoostConsumeRate * DeltaTime;
			CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);
			
		}
		else if(!bBoosting && bIsGrounded == true)
		{
			// Boost 回復
			CurrentBoost += BoostRecoverRate * DeltaTime;
			CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);
		}

		if (bIsBoosting && CurrentBoost <= 0.0f)
		{
			LandingHeight = GetActorLocation().Z;
			bIsBoosting = false;
			bIsRising = false;
			BoostAudioComponent->Stop();
			if (ActiveBoostEffect)
			{
				ActiveBoostEffect->DestroyComponent();
				ActiveBoostEffect = nullptr;
			}
		}
	}
	CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);

	// UI 更新
	if (BoostBar)
	{
		BoostBar->SetPercent(CurrentBoost / MaxBoost);
	}

	//場外に出たとき用の処理
	// 1000以上落ちたら着地リスポーン待機状態にする
	if (FallDis >= 400.0f && !bIsGrounded)
	{
		//bShouldRespawnAfterLanding = true;

		//GameOverUIを表示
		if (GameOverWidgetClass && GameOverWidgetInstance == nullptr)
		{
			GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
			if (GameOverWidgetInstance)
			{
				GameOverWidgetInstance->AddToViewport();
			}
		}
		// 入力を無効化
		DisableInput(PC);

		//  レベルリスタート
		FTimerHandle RespawnTimer;
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &AMyEgg::RespawnPlayer, RespawnDelay, false);
	}
	//通常のゲームオーバーの処理
	if ((LandingHeight - CurrentZ >= 400.0f) && !bIsGrounded)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()));

		//GameOverUIを表示
		if (GameOverWidgetClass && GameOverWidgetInstance == nullptr)
		{
			GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
			if (GameOverWidgetInstance)
			{
				GameOverWidgetInstance->AddToViewport();
			}
		}
		// 入力を無効化
		DisableInput(PC);
		//  レベルリスタート
		FTimerHandle RestartTimer;
		GetWorldTimerManager().SetTimer(RestartTimer, this, &AMyEgg::RespawnPlayer, RespawnDelay, false);

	}

	// 接地判定
	FVector Start = MeshComp->GetComponentLocation();
	FVector End = Start - FVector(0, 0, GroundCheckDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);


	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// Boostエフェクトの位置を更新
	if (ActiveBoostEffect)
	{
		ActiveBoostEffect->SetWorldLocation(MeshComp->GetComponentLocation() + BoostOffset);
		ActiveBoostEffect->SetWorldRotation(FRotator::ZeroRotator); // 回転固定
	}

	// Boost中なら上昇
	if (bIsRising && MeshComp)
	{
		FVector CurrentVelocity = MeshComp->GetPhysicsLinearVelocity();
		// Z方向に上昇速度を追加
		CurrentVelocity.Z = BoostRiseSpeed;
		MeshComp->SetPhysicsLinearVelocity(CurrentVelocity);
	}

	// 接地判定
	if (bHit)
	{
		bIsGrounded = true;
	}
	else
	{
		bIsGrounded = false;
	}

	// 接地したら回転を停止
	if (bIsGrounded)
	{
		bShouldRotateInAir = false;
	}
}
void AMyEgg::RespawnPlayer()
{

	if (!RespawnPoint.IsZero())
	{
		//LandingHeight = GetActorLocation().Z;
		SetActorLocation(RespawnPoint);
	}
	else
	{
		// チェックポイント未取得 → レベル再読込で確実に初期化
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		return;
	}
	// 一瞬だけ物理OFF
	MeshComp->SetSimulatePhysics(false);
	// 物理の吹っ飛び防止（速度リセット）
	if (MeshComp)
	{
		MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
	// すぐに物理ON
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
		{
			MeshComp->SetSimulatePhysics(true);
		}, 0.1f, false);
	// --- UI を削除 ---
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}

	// --- 入力を再有効化 ---
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		// DisableInput を使っていたなら EnableInput を呼ぶ
		EnableInput(PC);

		// 入力モードをゲームに戻す
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;

		// ポーズ解除（もし SetPause(true) を使っていたら）
		PC->SetPause(false);
	}
	LandingHeight = GetActorLocation().Z;
	// --- ステート・フラグのリセット ---
	bIsGrounded = false;           // 必要に応じて初期化
	bShouldRotateInAir = false;
	bIsRising = false;

}

void AMyEgg::SetCheckpoint(const FVector& NewLocation)
{
	RespawnPoint = NewLocation;
}

void AMyEgg::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);


	// 接地判定：Z方向がほぼ上向きの面に接触した場合
	if (HitNormal.Z > 0.01f)//0.7
	{
		bIsGrounded = true;   // 地面についている
		//着地地点を記録
		LandingHeight = GetActorLocation().Z;

		// バウンド防止
		FVector Vel = MeshComp->GetPhysicsLinearVelocity();
		Vel.Z = 0.0f;
		MeshComp->SetPhysicsLinearVelocity(Vel);
	}
}

// Called to bind functionality to input
void AMyEgg::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(ControlAction, ETriggerEvent::Triggered, this, &AMyEgg::ControlBall);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyEgg::Look);

		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &AMyEgg::BoostStart);

		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &AMyEgg::BoostStop);
	}
}

void AMyEgg::OnGoalReached()
{
	if (bIsGoalReached) return; // 二重判定防止
	bIsGoalReached = true;

	//動きを止める
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	MeshComp->SetSimulatePhysics(false); // ← 完全停止！
}


void AMyEgg::ControlBall(const FInputActionValue& Value)
{
	if (bIsGoalReached) return;

	FVector2D MoveValue = Value.Get<FVector2D>();
	if (!Controller || MoveValue.IsNearlyZero()) return;

	// カメラ方向に合わせた移動方向を計算
	FRotator CameraRot = Camera->GetComponentRotation();
	FVector Forward = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::X);
	FVector Right = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Y);
	LastMoveInput = MoveValue;
	Forward.Z = 0.0f;
	Right.Z = 0.0f;
	Forward.Normalize();
	Right.Normalize();

	FVector MoveDir = (Forward * MoveValue.Y + Right * MoveValue.X).GetSafeNormal();

	FVector CurrentVel = MeshComp->GetPhysicsLinearVelocity();
	FVector FlatVel = FVector(CurrentVel.X, CurrentVel.Y, 0.0f);

	float AccelerationBoost = 2.5f;//進む方向に力を加えてる

	// --- 逆方向入力時の減速処理 ---
	if (!FlatVel.IsNearlyZero())
	{
		float Dot = FVector::DotProduct(FlatVel.GetSafeNormal(), MoveDir);

		if (Dot < -0.5f) // ←真逆に近い方向を押したら
		{
			// 徐々に減速（0.85で減速率を調整）
			FVector NewVel = FlatVel * 0.85f;        

			// 少しブレーキをかけるが完全には止めない
			MeshComp->SetPhysicsLinearVelocity(FVector(NewVel.X, NewVel.Y, CurrentVel.Z));

			// ほんの少しだけ逆方向に力を加えて反転を始める
			float ControlStrength = bIsGrounded ? 0.5f : AirControlFactor * 0.5f;
			MeshComp->AddForce(MoveDir * Speed * MeshComp->GetMass() * ControlStrength * AccelerationBoost);

			return; // このフレームではこれで終わり
		}
	}

	// --- 通常の移動処理 ---
	if (FlatVel.Size() < 500.0f)
	{
		float ControlStrength = bIsGrounded ? 1.0f : AirControlFactor;
		MeshComp->AddForce(MoveDir * Speed * MeshComp->GetMass() * ControlStrength * AccelerationBoost);
	}

}


void AMyEgg::Look(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	const FVector2D V = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(V.X);
		AddControllerPitchInput(-V.Y);

		// Pawnが持っているControlの角度を取得する
		FRotator ControlRotate = GetControlRotation();
	}
}

void AMyEgg::BoostStart(const FInputActionValue& Value)
{
	if (CurrentBoost <= 0.0f)
		return;

	bIsBoosting = true;
	bIsRising = true;

	if (BoostAudioComponent && !BoostAudioComponent->IsPlaying())
	{
		BoostAudioComponent->Play();
	}

	// エフェクト開始
	if (BoostEffect && !ActiveBoostEffect)
	{
		// NiagaraをSpawn（アタッチせずにワールドに置く）
		ActiveBoostEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BoostEffect,
			MeshComp->GetComponentLocation() + BoostOffset,
			FRotator(0.0f, 0.0f, 100.0f),
			FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease
		);
	}
}

void AMyEgg::BoostStop(const FInputActionValue& Value)
{
	if (!bIsBoosting) return;
	bIsBoosting = false;
	bIsRising = false;
	LandingHeight = GetActorLocation().Z;
	if (ActiveBoostEffect)
	{
		ActiveBoostEffect->DestroyComponent();
		ActiveBoostEffect = nullptr;
	}

	if (BoostAudioComponent && BoostAudioComponent->IsPlaying())
	{
		BoostAudioComponent->Stop();
		//BoostAudioComponent->FadeOut(0.1f, 0.0f); // 急に切れないようにフェード
	}
}
