// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MGSpawnVolume.h"

#include "Components/BoxComponent.h"

AMGSpawnVolume::AMGSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

FVector AMGSpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return FMath::RandPointInBox(FBox(BoxOrigin - BoxExtent, BoxOrigin + BoxExtent));
}

AActor* AMGSpawnVolume::SpawnRandomItem()
{
	if (FMGItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FMGItemSpawnRow* AMGSpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FMGItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	
	ItemDataTable->GetAllRows(ContextString,AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FMGItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulatedChance = 0.0f;

	for (FMGItemSpawnRow* Row: AllRows)
	{
		if (Row)
		{
			AccumulatedChance += Row->SpawnChance;
			if (RandValue <= AccumulatedChance)
			{
				return Row;
			}
		}
	}

	return nullptr;
}

void AMGSpawnVolume::SetItemDataTable(UDataTable* NewDataTable)
{
	ItemDataTable = NewDataTable;
}

AActor* AMGSpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);

	return SpawnedActor;
}
