// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemInteractable.h"
#include "MGItemInteractableFog.generated.h"

class UExponentialHeightFogComponent;
class AMGCharacter;

/**
 * 상호작용 시 플레이어에게 안개 효과를 적용하는 아이템
 */
UCLASS(Blueprintable)
class MINIGAME_API AMGItemInteractableFog : public AMGItemInteractable
{
	GENERATED_BODY()
	
public:	
	AMGItemInteractableFog();

protected:
	// 안개 효과 설정값들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog Effect")
	float FogDensity = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog Effect")
	float FogDuration = 5.0f;

	// 안개 효과 관련 변수들
	float OriginalFogDensity = 0.0f;
	FTimerHandle FogTimerHandle;
	bool bIsUsed = false; // 아이템이 이미 사용되었는지 추적
	
	// FogComponent 참조 저장
	UPROPERTY()
	TObjectPtr<UExponentialHeightFogComponent> FogComponent;

	// 안개 효과 함수들
	void ApplyFogEffect();
	void RestoreFogDensity();

	// FPInteractable Interface 구현 - 안개 효과 적용
	virtual void Interact_Implementation(AMGCharacter* Interactor) override;

	// 아이템 활성화 시 추가 로직 (필요시 오버라이드)
	virtual void ActivateItem(AActor* Activator) override;
};



