// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MGItemCoinSmall.h"

AMGItemCoinSmall::AMGItemCoinSmall()
{
	CoinValue = 10; 
	ItemType = "SmallCoin";
}

void AMGItemCoinSmall::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}
