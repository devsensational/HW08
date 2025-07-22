#include "Item/MGItemInteractableFog.h"
#include "MGCharacter.h"
#include "Engine/Engine.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "ThirdParty/Interactable/FPInteractableComponent.h"

AMGItemInteractableFog::AMGItemInteractableFog()
{
	PrimaryActorTick.bCanEverTick = false;

	// 기본 안개 효과 설정값
	FogDensity = 0.8f;
	FogDuration = 5.0f;

	// 아이템 사용 여부 초기화
	bIsUsed = false;
}

void AMGItemInteractableFog::Interact_Implementation(AMGCharacter* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("MGItemInteractableFog::Interact_Implementation called"));
	
	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: Invalid Interactor"));
		return;
	}

	// 이미 사용된 아이템인지 확인
	if (bIsUsed)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: Item already used, ignoring interaction"));
		return;
	}

	// 사용됨으로 표시
	bIsUsed = true;

	// 안개 효과 적용
	ApplyFogEffect();
	
	UE_LOG(LogTemp, Log, TEXT("Applied fog effect to %s for %.1f seconds (Fog Density: %.1f)"), 
		   *Interactor->GetName(), FogDuration, FogDensity);

	// Cast to AActor for ActivateItem compatibility
	if (AActor* InteractorActor = Cast<AActor>(Interactor))
	{
		// 아이템 활성화 (부모 클래스의 로직 실행)
		ActivateItem(InteractorActor);
	}
}

void AMGItemInteractableFog::ActivateItem(AActor* Activator)
{
	// 부모 클래스의 ActivateItem 호출 (점수, 이펙트 등 기본 로직)
	Super::ActivateItem(Activator);
	
	// 메시를 숨기고 상호작용 컴포넌트 비활성화
	if (Mesh)
	{
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	// 상호작용 컴포넌트 비활성화
	if (InteractableComponent)
	{
		InteractableComponent->SetComponentTickEnabled(false);
		InteractableComponent->SetActive(false);
	}
	
	UE_LOG(LogTemp, Log, TEXT("MGItemInteractableFog: Item mesh hidden and interaction disabled"));
	
	// 안개 효과가 끝난 후에 파괴되도록 수정 (DestroyItem() 제거)
	// RestoreFogDensity()에서 파괴할 예정
}

void AMGItemInteractableFog::ApplyFogEffect()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: World is null"));
		return;
	}

	// 월드에서 ExponentialHeightFog 찾기
	AExponentialHeightFog* HeightFog = nullptr;
	for (TActorIterator<AExponentialHeightFog> ActorItr(World); ActorItr; ++ActorItr)
	{
		HeightFog = *ActorItr;
		break; // 첫 번째 찾은 안개 액터 사용
	}

	if (!HeightFog)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: No ExponentialHeightFog found in world"));
		return;
	}

	FogComponent = HeightFog->GetComponent();
	if (!FogComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: FogComponent is null"));
		return;
	}

	// 기존 타이머가 활성화되어 있다면 취소
	if (GetWorldTimerManager().IsTimerActive(FogTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FogTimerHandle);
	}

	// 원래 안개 밀도 저장
	OriginalFogDensity = FogComponent->FogDensity;

	// 새로운 안개 밀도 적용
	FogComponent->SetFogDensity(FogDensity);

	// 지정된 시간 후에 원래 안개 밀도로 복구하는 타이머 설정
	GetWorldTimerManager().SetTimer(FogTimerHandle, this, &AMGItemInteractableFog::RestoreFogDensity, FogDuration, false);

	UE_LOG(LogTemp, Log, TEXT("Fog density changed from %.3f to %.3f for %.1f seconds"), 
		   OriginalFogDensity, FogDensity, FogDuration);
}

void AMGItemInteractableFog::RestoreFogDensity()
{
	if (!FogComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("MGItemInteractableFog: FogComponent is null during restore"));
		return;
	}

	// 원래 안개 밀도로 복구
	FogComponent->SetFogDensity(OriginalFogDensity);

	UE_LOG(LogTemp, Log, TEXT("Fog density restored to %.3f"), OriginalFogDensity);

	// 아이템 파괴
	DestroyItem();
}


