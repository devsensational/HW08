// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MGItemCoin.h"

#include "Game/MGGameState.h"

AMGItemCoin::AMGItemCoin()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	
	CoinValue = 0;
	ItemType = "DefaultCoin";
	RotationSpeed = 90.0f; // 기본 회전 속도 (도/초)
}

void AMGItemCoin::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	RotateCoin(DeltaSeconds);
}

void AMGItemCoin::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = Activator->GetWorld())
		{
			if (AMGGameState* GameState = World->GetGameState<AMGGameState>())
			{
				GameState->AddScore(CoinValue);
				GameState->OnCoinCollected();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("MGItemCoin::ActivateItem - GameState not found!"));
			}
		}
		DestroyItem();
	}
}

void AMGItemCoin::RotateCoin(float DeltaSeconds)
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = CurrentRotation + FRotator(0.0f, RotationSpeed * DeltaSeconds, 0.0f);
	SetActorRotation(NewRotation);
}

