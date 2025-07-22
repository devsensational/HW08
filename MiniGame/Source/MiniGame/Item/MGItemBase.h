// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ItemInterface.h"
#include "MGItemBase.generated.h"

class USphereComponent;

UCLASS()
class MINIGAME_API AMGItemBase : public AActor, public IItemInterface
{
	GENERATED_BODY()
	
public:	
	AMGItemBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	TObjectPtr<USceneComponent> Scene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	TObjectPtr<USphereComponent> Collision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TObjectPtr<UParticleSystem> PickupParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	TObjectPtr<USoundBase> PickupSound;

public:
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;

	virtual void DestroyItem();

	// 게임 정리 이벤트에 응답하는 함수
	UFUNCTION()
	void OnGameCleanup();

	// 델리게이트 구독을 해제하기 위한 함수
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};