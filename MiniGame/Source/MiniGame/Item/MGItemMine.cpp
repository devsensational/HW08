// Fill out your copyright notice in the Description page of Project Settings.


#include "MGItemMine.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AMGItemMine::AMGItemMine()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;
	ItemType = "Mine";
	bHasExploded = false;
	
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetSphereRadius(300.0f);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMGItemMine::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	
	Super::ActivateItem(Activator);
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		5,
		FColor::Green,
		FString::Printf(TEXT("Mine Activated!")));
	
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMGItemMine::Explode,
		ExplosionDelay,
		false);

	bHasExploded = true;
}

void AMGItemMine::Explode()
{
	UParticleSystemComponent* Particle  = nullptr;
	
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
			);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
			);
	}
	
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor: OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr, // Instigator
				this, // Damage Causer
				UDamageType::StaticClass() // Damage Type
				);
		}
	}
	
	DestroyItem();

	if (Particle)
	{
		FTimerHandle DestoryParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestoryParticleTimerHandle,
			[Particle]()
			{
				Particle->DestroyComponent();
			},
			2.0f,
			false
		);
	}
}