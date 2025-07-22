// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameInstance.h"


UMGGameInstance::UMGGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UMGGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total score: %d"), TotalScore);
}


