// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemBase.h"
#include "MGItemHealing.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMGItemHealing : public AMGItemBase
{
	GENERATED_BODY()

public:
	AMGItemHealing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	int32 HealAmount;
	
	virtual void ActivateItem(AActor* Activator) override;
};
