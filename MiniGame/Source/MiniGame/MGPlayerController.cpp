// Fill out your copyright notice in the Description page of Project Settings.


#include "MGPlayerController.h"
#include "EnhancedInputSubsystems.h" // Enhanced Input System의 Local Player Subsystem을 사용하기 위해 포함
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Game/MGGameInstance.h"
#include "Game/MGGameState.h"
#include "Kismet/GameplayStatics.h"
#include "MGCharacter.h" // MGCharacter 헤더 추가

AMGPlayerController::AMGPlayerController()
	:	InputMappingContext(nullptr),
		MoveAction(nullptr),
		JumpAction(nullptr),
		LookAction(nullptr),
		TryInteractAction(nullptr),
		HUDWidgetClass(nullptr),
		HUDWidgetInstance(nullptr),
		MainMenuWidgetClass(nullptr),
		MainMenuWidgetInstance(nullptr)
{
	
}

UUserWidget* AMGPlayerController::GetHUDWidget()
{
	return HUDWidgetInstance;
}

void AMGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
			UE_LOG(LogTemp, Warning, TEXT("AHWCharacter::LocalPlayer Loaded"));
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			UE_LOG(LogTemp, Warning, TEXT("AHWCharacter::SubSystem Loaded"));
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0); // 0은 우선순위, 낮을수록 높은 우선순위
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MainMenu"))
	{
		ShowMainMenu(false);
	}
}

void AMGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("AHWPlayerController::OnPossess: Pawn %s possessed"), *InPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AHWPlayerController::OnPossess: No Pawn to possess"));
	}
}


void AMGPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
			
			// HUD 생성 후 캐릭터의 현재 HP로 초기화
			if (AMGCharacter* MGCharacter = Cast<AMGCharacter>(GetPawn()))
			{
				// 캐릭터의 OnHealthChanged 델리게이트가 있다면 즉시 브로드캐스트하여 HUD 업데이트
				MGCharacter->OnHealthChanged.Broadcast(MGCharacter->GetCurrentHealth(), MGCharacter->MaxHealth);
			}
		}

		AMGGameState* MGGameState = GetWorld() ? GetWorld()->GetGameState<AMGGameState>() : nullptr;
		if (MGGameState)
		{
			MGGameState->UpdateHUD();
		}
	}
}

void AMGPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}

	if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
	{
		if (bIsRestart)
		{
			ButtonText->SetText(FText::FromString(TEXT("Restart")));
		}
		else
		{
			ButtonText->SetText(FText::FromString(TEXT("Start")));

		}
	}

	if (bIsRestart)
	{
		if (UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim")))
		{
			MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}

		if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
		{
			if (UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), MGGameInstance->TotalScore)));
			}
		}
	}
}

void AMGPlayerController::StartGame()
{
	if (UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MGGameInstance->CurrentLevelIndex = 0; // 게임 시작 시 레벨 인덱스 초기화
		MGGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("BasicLevel")); // 기본 레벨로 이동
	SetPause(false);
}

void AMGPlayerController::ExitGame()
{
	FString CurrentMapName = GetWorld()->GetMapName();
	
	if (CurrentMapName.Contains("MainMenu"))
	{
		// MainMenu 레벨에서는 게임 종료
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->ConsoleCommand("quit");
		}
	}
	else
	{
		// 게임 진행 중에는 MainMenu 레벨로 돌아가기
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
	}
}
