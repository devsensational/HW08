// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MGItemBase.h"
#include "ThirdParty/Interactable/FPInteractable.h"
#include "ThirdParty/Interactable/FPHasMesh.h"
#include "MGItemInteractable.generated.h"

class UFPInteractableComponent;
class AFPCharacterBase;
class UWidgetComponent;

/**
 * MGItemBase를 상속받으면서 상호작용이 가능한 아이템 클래스
 */
UCLASS(Blueprintable)
class MINIGAME_API AMGItemInteractable : public AMGItemBase, public IFPInteractable, public IFPHasMesh
{
	GENERATED_BODY()
	
public:	
	AMGItemInteractable();

	// UI 표시 관련 함수들 - FPInteractableComponent에서 호출하기 위해 public으로 이동
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInteractionUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInteractionUI();

protected:
	virtual void BeginPlay() override;

	/* 상호작용 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<UFPInteractableComponent> InteractableComponent;

	// FPInteractable Interface 구현
	virtual void Interact_Implementation(AMGCharacter* Interactor) override;

	// FPHasMesh Interface 구현 - MGItemBase의 Mesh 컴포넌트 반환
	FORCEINLINE virtual UMeshComponent* GetMainMesh() override { return Mesh; }

	// 상호작용과 일반 오버랩을 구분하기 위한 플래그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRequiresInteraction = true;

	// 상호작용 우선 - 오버랩 비활성화 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bDisableOverlapWhenInteractable = true;

	/* 3D UI 섹션 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> InteractionUIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InteractionUIWidgetClass;
};