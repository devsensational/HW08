// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemBase.h"
#include "MGItemCoin.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMGItemCoin : public AMGItemBase
{
	GENERATED_BODY()

public:
	AMGItemCoin();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 CoinValue;

	virtual void ActivateItem(AActor* Activator) override;

	/* 비쥬얼 관련 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float RotationSpeed;
	
	virtual void RotateCoin(float DeltaSeconds);
};
