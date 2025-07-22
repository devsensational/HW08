// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MGGameState.generated.h"

// 게임 종료 시 아이템들을 정리하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameCleanup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUIChange);

UCLASS()
class MINIGAME_API AMGGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMGGameState();

	virtual void BeginPlay() override;

	// 게임 정리 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameCleanup OnGameCleanup;
	
	// 레벨 애니메이션 실행 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameCleanup OnLevelUIChange;
	
	/* Level 관련 섹션 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Coin")
	int32 SpawnedCoinCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Coin")
	int32 CollectedCoinCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Level")
	float LevelDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Level")
	int32 CurrentLevelIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Level")
	int32 MaxLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Level")
	TArray<FName> LevelMapNames;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<UDataTable> LevelDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	int32 ItemToSpawn;
	
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDTimerHandle;
	
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	
	UFUNCTION()
	void InitLevelData();
	
	void OnCoinCollected();
	void OnLevelTimeUp();
	void StartLevel();
	void EndLevel();
	void UpdateHUD();
	void ShowWaveCount();
};
