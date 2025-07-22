// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "MGCharacter.generated.h"

class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class UFPInteractableComponent;
class AMGItemInteractable;
// Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

// 체력 변경 이벤트를 위한 델리게이트 선언 (CurrentHealth, MaxHealth)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

UCLASS()
class MINIGAME_API AMGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMGCharacter();

	// 체력 변경 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 상호작용 레이캐스팅 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 200.0f;
	
	void PerformInteractionTrace();

private:
	// 마지막으로 하이라이트된 상호작용 컴포넌트
	UPROPERTY()
	TObjectPtr<UFPInteractableComponent> LastHighlightedComp;

	// 이동 속도 관련 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float NormalSpeedMultiplier;

	// 임시 속도 부스트 관련 변수들
	FTimerHandle SpeedBoostTimerHandle;
	float OriginalMultiplier;

	// 속도 부스트 종료 함수
	void EndSpeedBoost();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* 카메라 관련 섹션 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> FollowCamera;

	/* UI 관련 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> OverheadWidget;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitUI();
	
	void UpdateOverheadHP(); 
	
	/* Input 관련 섹션 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// IA_Move와 IA_Jump 등을 처리할 함수 원형
	// Enhanced Input에서 액션 값은 FInputActionValue로 전달됩니다.
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void TryInteract(const FInputActionValue& value);

	/* Status 관련 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHealth;

public:
	UFUNCTION(BlueprintPure, Category="Status")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category="Status")
	void AddHealth(float Amount);

	// NormalSpeedMultiplier 관련 함수들
	UFUNCTION(BlueprintPure, Category="Movement")
	float GetNormalSpeedMultiplier() const;

	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetNormalSpeedMultiplier(float NewMultiplier);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void UpdateMovementSpeed();

	// 임시 속도 부스트 함수
	UFUNCTION(BlueprintCallable, Category="Movement")
	void ApplyTemporarySpeedBoost(float SpeedMultiplier, float Duration);

protected:
	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
	void OnDeath();
};
