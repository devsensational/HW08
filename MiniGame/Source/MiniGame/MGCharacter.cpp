#include "MGCharacter.h"
#include "MGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Game/MGGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Game/MGCollision.h"
#include "ThirdParty/Interactable/FPInteractableComponent.h"

AMGCharacter::AMGCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Tick을 활성화하여 레이캐스팅 수행

	// 스프링 암 세팅
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
	SpringArm->bUsePawnControlRotation = true; // 캐릭터의 회전에 따라 카메라가 회전하도록 설정

	// 카메라 컴포넌트 세팅
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라가 캐릭터의 회전에 영향을 받지 않도록 설정

	// UI 위젯 컴포넌트 세팅
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	// 이동 속도 관련 초기값 설정
	NormalSpeed = 600.0f;
	NormalSpeedMultiplier = 1.0f;
	OriginalMultiplier = 1.0f;
	
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void AMGCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
	
	// 초기 체력 값으로 델리게이트 브로드캐스트
	InitUI();
	
	// 초기 이동 속도 설정
	UpdateMovementSpeed();
}

void AMGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임마다 상호작용 레이캐스팅 수행
	PerformInteractionTrace();
}

void AMGCharacter::InitUI()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AMGCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth)));
	}
}

void AMGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("PlayerInputComponent Class: %s"), 
		   *PlayerInputComponent->GetClass()->GetName());
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
        UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Cast Success"));
		if (const AMGPlayerController* PlayerController = Cast<AMGPlayerController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Controller Loaded"));
			if (PlayerController -> MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AMGCharacter::Move);
			}
			if (PlayerController->JumpAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController JumpAction"));
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AMGCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AMGCharacter::StopJump);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AMGCharacter::Look);
			}
			if (PlayerController->TryInteractAction)
			{
				EnhancedInput->BindAction(PlayerController->TryInteractAction, ETriggerEvent::Triggered, this, &AMGCharacter::TryInteract);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Cast Failed"));
	}
}

void AMGCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}

}

void AMGCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
		UE_LOG(LogTemp, Warning, TEXT("Jump Started"));
	}
}

void AMGCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AMGCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AMGCharacter::TryInteract(const FInputActionValue& value)
{
	if (!value.Get<bool>() || !IsLocallyControlled()) return;

	// 상호작용을 위한 레이캐스트 발사
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * InteractionRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CCHANNEL_MGINTERACT, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UFPInteractableComponent* InteractableComp = HitActor->FindComponentByClass<UFPInteractableComponent>();
			if (InteractableComp)
			{
				// FPInteractableComponent를 통해 상호작용 시도
				InteractableComp->Interact(this);
				UE_LOG(LogTemp, Log, TEXT("AMGCharacter::TryInteract - Interacted with %s"), *HitActor->GetName());
			}
		}
	}
}

float AMGCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

void AMGCharacter::AddHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
	
	// 체력 변경 시 델리게이트 브로드캐스트
	InitUI();
}

float AMGCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();
	
	// 체력 변경 시 델리게이트 브로드캐스트
	InitUI();

	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}
	return ActualDamage;
}

void AMGCharacter::OnDeath()
{
	// Todo: 캐릭터가 죽었을 때의 처리
	if (AMGGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AMGGameState>() : nullptr)
	{
		GameState->OnGameOver();
	}
}

void AMGCharacter::PerformInteractionTrace()
{
	/* 상호작용을 위한 Ray 발사 섹션 */
	if (IsLocallyControlled())
	{
		FVector Start = FollowCamera->GetComponentLocation();
		FVector End = Start + FollowCamera->GetForwardVector() * InteractionRange;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CCHANNEL_MGINTERACT, Params))
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				UFPInteractableComponent* InteractableComp = HitActor->FindComponentByClass<UFPInteractableComponent>();

				if (InteractableComp)
				{
					// 이전 대상과 다르면 전 대상은 끄기
					if (LastHighlightedComp && LastHighlightedComp != InteractableComp)
					{
						LastHighlightedComp->Highlight(false);
					}

					// 새 대상 Highlight 켜기
					InteractableComp->Highlight(true);
					LastHighlightedComp = InteractableComp;
					return;
				}
			}
		}

		// 부딪힌게 없거나 상호작용 대상이 아니면
		if (LastHighlightedComp)
		{
			LastHighlightedComp->Highlight(false);
			LastHighlightedComp = nullptr;
		}
	}
}

// NormalSpeedMultiplier 관련 함수 구현
float AMGCharacter::GetNormalSpeedMultiplier() const
{
	return NormalSpeedMultiplier;
}

void AMGCharacter::SetNormalSpeedMultiplier(float NewMultiplier)
{
	NormalSpeedMultiplier = FMath::Max(0.1f, NewMultiplier); // 최소값 0.1로 제한
	UpdateMovementSpeed();
}

void AMGCharacter::UpdateMovementSpeed()
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// 기본 이동 속도에 NormalSpeedMultiplier 적용 (Sprint 관련 코드 제거)
		float AdjustedSpeed = NormalSpeed * NormalSpeedMultiplier;
		MovementComp->MaxWalkSpeed = AdjustedSpeed;
		
		UE_LOG(LogTemp, Log, TEXT("Movement Speed Updated - Speed: %.1f (Multiplier: %.2f)"), 
			   AdjustedSpeed, NormalSpeedMultiplier);
	}
}

// 임시 속도 부스트 함수 구현
void AMGCharacter::ApplyTemporarySpeedBoost(float SpeedMultiplier, float Duration)
{
	// 현재 타이머가 활성화되어 있다면 취소
	if (GetWorldTimerManager().IsTimerActive(SpeedBoostTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(SpeedBoostTimerHandle);
	}
	
	// 현재 배율을 저장하고 새로운 배율 적용
	SetNormalSpeedMultiplier(SpeedMultiplier);
	
	// 지정된 시간 후에 원래 속도로 복구하는 타이머 설정
	GetWorldTimerManager().SetTimer(SpeedBoostTimerHandle, this, &AMGCharacter::EndSpeedBoost, Duration, false);
	
	UE_LOG(LogTemp, Log, TEXT("Applied temporary speed boost: %.2fx for %.1f seconds"), SpeedMultiplier, Duration);
}

void AMGCharacter::EndSpeedBoost()
{
	// 원래 배율로 복구
	SetNormalSpeedMultiplier(OriginalMultiplier);
	
	UE_LOG(LogTemp, Log, TEXT("Speed boost ended, returned to normal speed (%.2fx)"), OriginalMultiplier);
}



