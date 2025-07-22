// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/MGItemInteractable.h"
#include "ThirdParty/Interactable/FPInteractableComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "MGCharacter.h"

AMGItemInteractable::AMGItemInteractable()
{
	// InteractableComponent 생성 및 설정
	InteractableComponent = CreateDefaultSubobject<UFPInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->SetupAttachment(RootComponent);
	
	// 3D UI 컴포넌트 생성 및 설정
	InteractionUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionUIComponent"));
	InteractionUIComponent->SetupAttachment(RootComponent);
	InteractionUIComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // 아이템 위쪽에 표시
	InteractionUIComponent->SetWidgetSpace(EWidgetSpace::Screen); // 3D 공간에 표시
	InteractionUIComponent->SetDrawSize(FVector2D(200.0f, 100.0f)); // UI 크기 설정
	InteractionUIComponent->SetVisibility(false); // 기본적으로 숨김
	
	// 기본값 설정
	bRequiresInteraction = true;
	bDisableOverlapWhenInteractable = true;
}

void AMGItemInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	// 상호작용 우선 모드일 때 기존 오버랩 비활성화
	if (bDisableOverlapWhenInteractable && bRequiresInteraction && Collision)
	{
		// 기존 오버랩 이벤트 언바인드
		Collision->OnComponentBeginOverlap.RemoveDynamic(this, &AMGItemBase::OnItemOverlap);
		Collision->OnComponentEndOverlap.RemoveDynamic(this, &AMGItemBase::OnItemEndOverlap);
		
		UE_LOG(LogTemp, Log, TEXT("MGItemInteractable: Overlap events disabled for interaction-only mode"));
	}
}

void AMGItemInteractable::Interact_Implementation(AMGCharacter* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("MGItemInteractable::Interact_Implementation called"));
	
	// Cast to AActor for ActivateItem compatibility
	if (AActor* InteractorActor = Cast<AActor>(Interactor))
	{
		// MGItemBase의 ActivateItem 호출
		//Interactor->ApplyTemporarySpeedBoost(2.0f, 3.0f);
		ActivateItem(InteractorActor);
	}
}

void AMGItemInteractable::ShowInteractionUI()
{
	if (InteractionUIComponent && InteractionUIWidgetClass)
	{
		// 위젯 클래스가 설정되어 있으면 위젯 생성
		if (!InteractionUIComponent->GetWidget())
		{
			InteractionUIComponent->SetWidgetClass(InteractionUIWidgetClass);
		}
		
		InteractionUIComponent->SetVisibility(true);
		UE_LOG(LogTemp, Log, TEXT("Interaction UI shown for %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionUIComponent or InteractionUIWidgetClass is null for %s"), *GetName());
	}
}

void AMGItemInteractable::HideInteractionUI()
{
	if (InteractionUIComponent)
	{
		InteractionUIComponent->SetVisibility(false);
		UE_LOG(LogTemp, Log, TEXT("Interaction UI hidden for %s"), *GetName());
	}
}
