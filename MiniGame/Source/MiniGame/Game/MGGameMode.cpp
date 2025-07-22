// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MGGameMode.h"

#include "MGCharacter.h"
#include "MGGameState.h"
#include "MGPlayerController.h"

AMGGameMode::AMGGameMode()
{
	DefaultPawnClass = AMGCharacter::StaticClass();
	PlayerControllerClass = AMGPlayerController::StaticClass();
	GameStateClass = AMGGameState::StaticClass();
}
