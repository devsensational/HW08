// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemInteractable.h"
#include "MGItemInteractableSpeedUp.generated.h"

class AMGCharacter;

UCLASS(Blueprintable)
class MINIGAME_API AMGItemInteractableSpeedUp : public AMGItemInteractable
{
	GENERATED_BODY()
	
public:	
	AMGItemInteractableSpeedUp();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Boost")
	float SpeedMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed Boost")
	float BoostDuration = 3.0f;

	// FPInteractable Interface 구현 - 속도 부스트 적용
	virtual void Interact_Implementation(AMGCharacter* Interactor) override;

	virtual void ActivateItem(AActor* Activator) override;
};
