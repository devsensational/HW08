// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemCoin.h"
#include "MGItemCoinSmall.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMGItemCoinSmall : public AMGItemCoin
{
	GENERATED_BODY()

public:
	AMGItemCoinSmall();

	virtual void ActivateItem(AActor* Activator) override;
};
