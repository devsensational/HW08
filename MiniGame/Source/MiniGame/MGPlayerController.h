// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMGPlayerController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "input")
	TObjectPtr<UInputAction> TryInteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> HUDWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> MainMenuWidgetInstance;
	
	
	UFUNCTION(BlueprintPure, Category = "UI")
	UUserWidget* GetHUDWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameHUD();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu(bool bIsRestart);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartGame();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ExitGame();
	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};
