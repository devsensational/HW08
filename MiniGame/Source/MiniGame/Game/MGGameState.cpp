// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameState.h"

#include "MGGameInstance.h"
#include "MGPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Item/MGItemCoin.h"
#include "Kismet/GameplayStatics.h"
#include "Level/MGSpawnVolume.h"
#include "Level/Data/MGLevelRow.h"

AMGGameState::AMGGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	ItemToSpawn = 40;
	LevelDuration = 40;
	CurrentLevelIndex = 0;
	MaxLevels = 4; // 기본값, BeginPlay에서 LevelDataTable 기반으로 업데이트됨
}

void AMGGameState::BeginPlay()
{
	Super::BeginPlay();

	// LevelDataTable의 행 개수로 MaxLevels 설정
	if (LevelDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelDataTable is valid, checking rows..."));
		
		TArray<FName> RowNames = LevelDataTable->GetRowNames();
		UE_LOG(LogTemp, Warning, TEXT("Found %d rows in LevelDataTable"), RowNames.Num());
		
		// 각 Row 이름을 로그로 출력
		for (int32 i = 0; i < RowNames.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Row %d: %s"), i, *RowNames[i].ToString());
		}
		
		if (RowNames.Num() > 0)
		{
			MaxLevels = RowNames.Num();
			UE_LOG(LogTemp, Warning, TEXT("MaxLevels set to %d based on LevelDataTable row count"), MaxLevels);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LevelDataTable has no rows! Using default MaxLevels: %d"), MaxLevels);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LevelDataTable is null! Using default MaxLevels: %d"), MaxLevels);
	}

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDTimerHandle,
		this,
		&AMGGameState::UpdateHUD,
		0.01f,
		true);
}

int32 AMGGameState::GetScore() const
{
	return Score;
}

void AMGGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GameInstance))
		{
			MGGameInstance->AddToScore(Amount);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Score updated: %d"), Score);
}

void AMGGameState::OnGameOver()
{
	// 게임 종료 시 모든 아이템들을 정리하기 위해 델리게이트 호출
	OnGameCleanup.Broadcast();
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMGPlayerController* MGPlayerController = Cast<AMGPlayerController>(PlayerController))
		{
			//MGPlayerController->SetPause(true);
			MGPlayerController->ShowMainMenu(true);
		}
	}
}

void AMGGameState::InitLevelData()
{
	if (!LevelDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelDataTable is not set!"));
		return;
	}

	// 현재 레벨 인덱스를 기반으로 행 이름 생성 (예: "Level_0", "Level_1", etc.)
	FName RowName = FName(*FString::Printf(TEXT("Level_%d"), CurrentLevelIndex));
	
	// 데이터 테이블에서 해당 레벨의 데이터 가져오기
	FMGLevelRow* LevelData = LevelDataTable->FindRow<FMGLevelRow>(RowName, TEXT("InitLevelData"));
	
	if (LevelData)
	{
		// LevelDuration 동기화
		LevelDuration = LevelData->LevelDuration;
		
		UE_LOG(LogTemp, Log, TEXT("Level %d data loaded - Duration: %.1f, MaxItems: %d"), 
			CurrentLevelIndex, LevelData->LevelDuration, LevelData->MaxItemCount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find level data for Level_%d"), CurrentLevelIndex);
		// 기본값 사용
		LevelDuration = 40.0f;
	}
}

void AMGGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Log, TEXT("Coin collected! Total collected: %d"), CollectedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		UE_LOG(LogTemp, Log, TEXT("All coins collected! Ending level..."));
		EndLevel();
	}
}

void AMGGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMGPlayerController* MGPlayerController = Cast<AMGPlayerController>(PlayerController))
		{
			MGPlayerController->ShowGameHUD();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GameInstance))
		{
			CurrentLevelIndex = MGGameInstance->CurrentLevelIndex;
		}
	}
	
	// 레벨 데이터 초기화
	InitLevelData();
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMGSpawnVolume::StaticClass(), FoundVolumes);

	// 스폰되는 아이템 갯수 - LevelDataTable에서 가져온 값 사용

	FMGLevelRow* LevelData = nullptr;
	if (LevelDataTable)
	{
		FName RowName = FName(*FString::Printf(TEXT("Level_%d"), CurrentLevelIndex));
		LevelData = LevelDataTable->FindRow<FMGLevelRow>(RowName, TEXT("StartLevel"));
		
		if (LevelData)
		{
			ItemToSpawn = LevelData->MaxItemCount;
		}
	}

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			AMGSpawnVolume* SpawnVolume = Cast<AMGSpawnVolume>(FoundVolumes[FMath::RandRange(0, FoundVolumes.Num() - 1)]);
			if (SpawnVolume)
			{
				if (LevelData)
				{
					SpawnVolume->SetItemDataTable(LevelData->ItemDataTable);
				}
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(AMGItemCoin::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}
	ShowWaveCount();
	// 레벨 시작 시 설명
	if (LevelData)
	{
		if (AMGPlayerController* MGPlayerController = Cast<AMGPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (UUserWidget* HUDWidget = MGPlayerController->GetHUDWidget())
			{
				if (UTextBlock* LevelDescriptionText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelDescription"))))
				{
					LevelDescriptionText->SetText(FText::FromString(LevelData->LevelDescription));
				}
				
				if (UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("LevelDescriptionAnim")))
				{
					HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
					UE_LOG(LogTemp, Warning, TEXT("LevelDescriptionAnim"));
				}
			}
		}
	}
	
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&AMGGameState::OnLevelTimeUp,
		LevelDuration,
		false
		);
	
	UE_LOG(LogTemp, Warning, TEXT("Level %d started with %d coins to collect."), CurrentLevelIndex + 1, SpawnedCoinCount);
}

void AMGGameState::EndLevel()
{
	// 레벨 종료 시 모든 아이템들을 정리하기 위해 델리게이트 호출
	OnGameCleanup.Broadcast();
	
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GameInstance))
		{
			AddScore(Score);
			CurrentLevelIndex++;
			MGGameInstance->CurrentLevelIndex = CurrentLevelIndex;
			
			UE_LOG(LogTemp, Warning, TEXT("EndLevel: CurrentLevelIndex incremented to %d, MaxLevels: %d"), CurrentLevelIndex, MaxLevels);
		}
	}
	
	if (CurrentLevelIndex >= MaxLevels)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Over: CurrentLevelIndex (%d) >= MaxLevels (%d)"), CurrentLevelIndex, MaxLevels);
		OnGameOver();
		return;
	}

	// LevelMapNames 배열 체크는 제거하고 바로 다음 레벨 시작
	UE_LOG(LogTemp, Warning, TEXT("Starting next level: %d"), CurrentLevelIndex);
	StartLevel();
}

void AMGGameState::OnLevelTimeUp()
{
	OnGameOver();
}

void AMGGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMGPlayerController* MGPlayerController = Cast<AMGPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MGPlayerController->GetHUDWidget())
			{
				//타이머 - 남은 시간 표시 (0.01초 단위)
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.2f"), RemainingTime)));
				}

				// 점수
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GameInstance);
						if (MGGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), MGGameInstance->TotalScore)));
						}
					}
				}
			}
		}
	}
}

void AMGGameState::ShowWaveCount()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMGPlayerController* MGPlayerController = Cast<AMGPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MGPlayerController->GetHUDWidget())
			{
				//레벨
				if (UTextBlock* LevleText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevleText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentLevelIndex + 1)));
					UE_LOG(LogTemp, Warning, TEXT("Current Level Index: %d"), CurrentLevelIndex);
				}
							
				if (UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("WaveStartAnim")))
				{
					HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
					//OnLevelUIChange.Broadcast();
					UE_LOG(LogTemp, Warning, TEXT("WaveStartAnim"));
				}

			}
		}
	}
}
