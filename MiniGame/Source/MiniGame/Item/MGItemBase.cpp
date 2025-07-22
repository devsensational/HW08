// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MGItemBase.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Game/MGGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMGItemBase::AMGItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

	// 이벤트 바인딩
	// 이벤트 바인딩의 이유
	// 이벤트는 런타임 상황에서 이루어지는데, 동적인 상황에서 어떤 객체가 충돌했는지 알 수 없기 때문에
	// 런타임에 해당 이벤트를 처리할 함수를 바인딩해야 합니다.
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMGItemBase::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AMGItemBase::OnItemEndOverlap);
}

void AMGItemBase::BeginPlay()
{
	Super::BeginPlay();

	// GameState의 OnGameCleanup 델리게이트에 구독
	if (UWorld* World = GetWorld())
	{
		if (AMGGameState* GameState = World->GetGameState<AMGGameState>())
		{
			GameState->OnGameCleanup.AddDynamic(this, &AMGItemBase::OnGameCleanup);
		}
	}
}

void AMGItemBase::OnItemOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Player Hit!");
		ActivateItem(OtherActor);
	}
}

void AMGItemBase::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex)
{
	
}

void AMGItemBase::ActivateItem(AActor* Activator)
{

	UParticleSystemComponent* Particle = nullptr;
	
	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation());
	}

	
	
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

FName AMGItemBase::GetItemType() const
{
	return ItemType;
}

void AMGItemBase::DestroyItem()
{
	Destroy();
}

void AMGItemBase::OnGameCleanup()
{
	// 게임 정리 시 아이템을 제거
	DestroyItem();
}

void AMGItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 델리게이트 구독 해제
	if (UWorld* World = GetWorld())
	{
		if (AMGGameState* GameState = World->GetGameState<AMGGameState>())
		{
			GameState->OnGameCleanup.RemoveDynamic(this, &AMGItemBase::OnGameCleanup);
		}
	}

	Super::EndPlay(EndPlayReason);
}
