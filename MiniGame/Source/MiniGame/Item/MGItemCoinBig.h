// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemCoin.h"
#include "MGItemCoinBig.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMGItemCoinBig : public AMGItemCoin
{
	GENERATED_BODY()

public:
	AMGItemCoinBig();

	virtual void ActivateItem(AActor* Activator) override;
};
