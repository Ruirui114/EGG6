// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEGG.h"

#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "MyCheckpointo.h"
#include "Engine/StaticMesh.h"
#include "Blueprint/UserWidget.h"
#include "MyWidget.h"
#include "ScoreWidget.h"
#include "MyGameInstance.h"
#include "EngineUtils.h"
#include "DoragonActor.h"
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

	SuperBoostAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/SuperBoost"));

	TabAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Tab"));
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


	if (SuperBoostSound)
	{
		SuperBoostAudioComponent = NewObject<UAudioComponent>(this);
		SuperBoostAudioComponent->SetupAttachment(RootComponent);
		SuperBoostAudioComponent->SetSound(SuperBoostSound);
		SuperBoostAudioComponent->bAutoActivate = false; // 自動再生 OFF
		SuperBoostAudioComponent->bIsUISound = false;
		SuperBoostAudioComponent->RegisterComponent();
	}

	if (PlayerMesh)
	{
		MeshComp->SetStaticMesh(PlayerMesh);
	}

	if (MeshComp && PhysicsMaterial)
	{
		MeshComp->SetPhysMaterialOverride(PhysicsMaterial);
	}

	if (!ScoreWidget && ScoreWidgetClass)
	{
		ScoreWidget = CreateWidget<UScoreWidget>(GetWorld(), ScoreWidgetClass);
		if (ScoreWidget)
		{
			ScoreWidget->AddToViewport();
			ScoreWidget->UpdateEggCount(); // 初期表示
		}
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		EnableInput(PC);
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
	
	FVector Start = MeshComp->GetComponentLocation();
	FVector End = Start - FVector(0.0f, 0.0f, GroundCheckDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);


	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	bIsGrounded = bHit;

	if (!bIsGrounded && !bIsFalling)
	{
		// 空中に出た瞬間
		bIsFalling = true;
		FallStartZ = GetActorLocation().Z;
	}

	if (bIsGrounded && bIsFalling)
	{
		// 着地
		bIsFalling = false;
	}

	if (bIsFalling && !bIsGrounded && !bIsGameOver)
	{
		CurrentZ = FallStartZ - GetActorLocation().Z;

		if (CurrentZ >= 800.0f)
		{
			bIsGameOver = true;

			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				DisableInput(PC);
			}

			if (GameOverWidgetClass && !GameOverWidgetInstance)
			{
				GameOverWidgetInstance =
					CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
				GameOverWidgetInstance->AddToViewport();
			}
			FTimerHandle RestartTimer;
			GetWorldTimerManager().SetTimer(
				RestartTimer,
				this,
				&AMyEgg::RespawnPlayer,
				RespawnDelay,
				false
			);
		}
	}

	// Boost ゲージ管理
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

		CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);
		// Boost 終了判定
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


	// UI 更新
	if (BoostBar)
	{
		BoostBar->SetPercent(CurrentBoost / MaxBoost);
	}

	// Boostエフェクトの位置を更新
	if (ActiveBoostEffect)
	{
		ActiveBoostEffect->SetWorldLocation(MeshComp->GetComponentLocation() + BoostOffset);
		ActiveBoostEffect->SetWorldRotation(FRotator::ZeroRotator); // 回転固定
	}
	if (ActiveSuperBoostEffect)
	{
		ActiveSuperBoostEffect->SetWorldLocation(MeshComp->GetComponentLocation() + BoostOffset);
		ActiveSuperBoostEffect->SetWorldRotation(FRotator::ZeroRotator); // 回転固定
	}
	// Boost中なら上昇
	if (bIsRising && MeshComp)
	{
		FVector CurrentVelocity = MeshComp->GetPhysicsLinearVelocity();
		// Z方向に上昇速度を追加
		CurrentVelocity.Z = BoostRiseSpeed;
		MeshComp->SetPhysicsLinearVelocity(CurrentVelocity);
	}

	// 接地したら回転を停止
	if (bIsGrounded)
	{
		bShouldRotateInAir = false;
	}
}

void AMyEgg::RespawnPlayer()
{
	CurrentBoost = MaxBoost;
	bIsFalling = false;
	bIsGameOver = false;
	bIsBoosting = false;
	FallStartZ = 0.0f;
	if (!RespawnPoint.IsZero())
	{
		SetActorLocation(RespawnPoint);
		//LandingHeight = GetActorLocation().Z;
		FallStartZ = RespawnPoint.Z;
	}
	else
	{
		if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
		{
			GI->ResetEggScore();
		}
		// チェックポイント未取得 → レベル再読込で確実に初期化
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

		return;
	}

	if (MeshComp)
	{
		// 速度だけリセット
		MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		// 念のため下向き初速を与える（重要）
		MeshComp->AddImpulse(FVector(0, 0, -50.0f), NAME_None, true);
	}

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

	// --- ステート・フラグのリセット ---
	bIsGrounded = false;           // 必要に応じて初期化
	bShouldRotateInAir = false;
	bIsRising = false;

}

void AMyEgg::SetCheckpoint(const FVector& NewLocation)
{
	RespawnPoint = NewLocation;
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

		EnhancedInputComponent->BindAction(SuperBoostAction, ETriggerEvent::Started, this, &AMyEgg::SuperJump);

		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &AMyEgg::Tab);
	}
}

void AMyEgg::OnGoalReached()
{
	if (bIsGoalReached) return; // 二重判定防止
	bIsGoalReached = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (SpringArm)
	{
		SpringArm->bEnableCameraLag = false;
		SpringArm->bEnableCameraRotationLag = false;
		SpringArm->TargetArmLength = SpringArm->TargetArmLength; // 固定
	}

	//ClearUIを表示
	if (ClearWidgetClass && ClearWidgetInstance == nullptr)
	{
		ClearWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClearWidgetClass);
		if (ClearWidgetInstance)
		{
			PC->bShowMouseCursor = true;
			ClearWidgetInstance->AddToViewport();
		}
	}
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		for (TActorIterator<ADoragonActor> It(GetWorld()); It; ++It)
		{
			It->SetRewardByScore(GI->EggScore);
		}
	}
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
	FallStartZ = GetActorLocation().Z;
	bIsFalling = true;
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

void AMyEgg::SuperJump()
{
	// 接地中のみ（空中連打防止）
	if (!bIsGrounded)
		return;
	if (!SuperBoostEffect) return;

	float Cost = MaxBoost * SuperJumpCostRatio;

	// ゲージ不足
	if (CurrentBoost < Cost)
		return;

	// ゲージ消費
	CurrentBoost -= Cost;
	CurrentBoost = FMath::Clamp(CurrentBoost, 0.0f, MaxBoost);

	// 上方向に強い力を加える
	FVector JumpForce = FVector(0.0f, 0.0f, SuperJumpForce);
	MeshComp->AddImpulse(JumpForce);

	// 接地フラグを即解除
	bIsGrounded = false;

	// UI更新
	if (MyWidgetInstance)
	{
		MyWidgetInstance->UpdateBoostBar(CurrentBoost, MaxBoost);
	}
	// サウンド再生
	if (SuperBoostAudioComponent && !SuperBoostAudioComponent->IsPlaying())
	{
		SuperBoostAudioComponent->Play();
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(
			Timer,
			[this]()
			{
				if (SuperBoostAudioComponent)
				{
					SuperBoostAudioComponent->Stop();
				}
			},
			0.6f,
			false
		);
	}

	// エフェクト開始
	UNiagaraComponent* SuperBoostComp =UNiagaraFunctionLibrary::SpawnSystemAttached
	(
		SuperBoostEffect,
		MeshComp,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		false   // AutoDestroy は false
	);

	if (SuperBoostComp)
	{
		// 1秒後に消す
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(
			Timer,
			[SuperBoostComp]()
			{
				if (SuperBoostComp)
				{
					SuperBoostComp->DestroyComponent();
				}
			},
			1.0f,
			false
		);
	}

}

void AMyEgg::Tab()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !MenuWidgetClass) return;

	MenuWidgetInstance = CreateWidget<UUserWidget>(PC, MenuWidgetClass);
	if (MenuWidgetInstance)
	{
		MenuWidgetInstance->AddToViewport();

		MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		MeshComp->SetSimulatePhysics(false);

		PC->SetInputMode(FInputModeUIOnly());
		PC->bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	// ゲーム停止（完全フリーズ）
	UGameplayStatics::SetGamePaused(GetWorld(), true);

}

void AMyEgg::ResumeFromMenu()
{
	// 物理再開
	MeshComp->SetSimulatePhysics(true);

	// 念のため速度ゼロ
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

void AMyEgg::AddEggCount()
{
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		GI->EggScore++; // スコア更新
	}

	if (ScoreWidget)
	{
		ScoreWidget->UpdateEggCount(); // 表示更新
	}
}