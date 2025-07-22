#include "Item/MGItemInteractableSpeedUp.h"
#include "MGCharacter.h"
#include "Engine/Engine.h"

AMGItemInteractableSpeedUp::AMGItemInteractableSpeedUp()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 기본 속도 부스트 설정값
	SpeedMultiplier = 2.0f;
	BoostDuration = 3.0f;
}

void AMGItemInteractableSpeedUp::Interact_Implementation(AMGCharacter* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("MGItemInteractableSpeedUp::Interact_Implementation called"));
	
	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableSpeedUp: Invalid Interactor"));
		return;
	}

	// 플레이어에게 속도 부스트 적용
	Interactor->ApplyTemporarySpeedBoost(SpeedMultiplier, BoostDuration);
	
	UE_LOG(LogTemp, Log, TEXT("Applied speed boost to %s: %.1fx for %.1f seconds"), 
		   *Interactor->GetName(), SpeedMultiplier, BoostDuration);

	// Cast to AActor for ActivateItem compatibility
	if (AActor* InteractorActor = Cast<AActor>(Interactor))
	{
		// 아이템 활성화 (부모 클래스의 로직 실행)
		ActivateItem(InteractorActor);
	}
}

void AMGItemInteractableSpeedUp::ActivateItem(AActor* Activator)
{
	// 부모 클래스의 ActivateItem 호출 (점수, 이펙트 등 기본 로직)
	Super::ActivateItem(Activator);
	
	// 속도 부스트 아이템은 사용 후 파괴
	DestroyItem();
}
