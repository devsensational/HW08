// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdParty/Interactable/FPInteractableComponent.h"

#include "FPHasMesh.h"
#include "MGCharacter.h"
#include "Game/MGCollision.h"
#include "Item/MGItemInteractable.h"


UFPInteractableComponent::UFPInteractableComponent()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionProfileName(CPROFILE_MGINTERACTTRIGGER);
	SetHiddenInGame(true);
	SetGenerateOverlapEvents(true);
	SetMobility(EComponentMobility::Movable);
	
	// 하이라이트 초기 상태를 비활성화로 설정
	SetRenderCustomDepth(false);
	SetCustomDepthStencilValue(0);
}

void UFPInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	Highlight(false);
	if (AActor* OwnerActor = GetOwner())
	{
		if (IFPHasMesh* HasMeshInterface = Cast<IFPHasMesh>(OwnerActor))
		{
			CachedMesh = HasMeshInterface->GetMainMesh();
			UE_LOG(LogTemp, Log, TEXT("Mesh cashed"));
		}
		else
		{
			// 폴백: 메시 직접 찾기
			CachedMesh = OwnerActor->FindComponentByClass<UMeshComponent>();
		}
	}
}

void UFPInteractableComponent::PostInitProperties()
{
	Super::PostInitProperties();
	
	SetBoxExtent(BoxSize);
}

void UFPInteractableComponent::Highlight(bool bEnable)
{
	if (!CachedMesh) return;

	CachedMesh->SetRenderCustomDepth(bEnable);

	// 필요 시 색상 지정 (Material에서 이 StencilValue를 기준으로 색상 분기)
	CachedMesh->SetCustomDepthStencilValue(bEnable ? 1 : 0);
	
	// MGItemInteractable의 3D UI 제어
	if (AActor* OwnerActor = GetOwner())
	{
		if (AMGItemInteractable* InteractableItem = Cast<AMGItemInteractable>(OwnerActor))
		{
			if (bEnable)
			{
				InteractableItem->ShowInteractionUI();
			}
			else
			{
				InteractableItem->HideInteractionUI();
			}
		}
	}
}

void UFPInteractableComponent::Interact(AMGCharacter* Interactor) const
{
	if (AActor* OwnerActor = GetOwner())
	{
		UE_LOG(LogTemp, Log, TEXT("%s 와 상호작용 실행"), *OwnerActor->GetName());

		// 캐릭터인지 확인
		if (OwnerActor->Implements<UFPInteractable>())
		{
			IFPInteractable::Execute_Interact(OwnerActor, Interactor);
		}
	}
}
