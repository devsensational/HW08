// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MGItemHealing.h"

#include "MGCharacter.h"

AMGItemHealing::AMGItemHealing()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AMGItemHealing::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(Activator))
		{
			PlayerCharacter->AddHealth(HealAmount);	
		}
		
		DestroyItem();
	}
}
