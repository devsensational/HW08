// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MGItemCoinBig.h"

AMGItemCoinBig::AMGItemCoinBig()
{
	CoinValue = 50;
	ItemType = "BigCoin";
}

void AMGItemCoinBig::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}
