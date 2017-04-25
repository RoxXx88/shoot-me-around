// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShootMeAround.h"
#include "ShootMeAroundGameMode.h"
#include "ShootMeAroundCharacter.h"

AShootMeAroundGameMode::AShootMeAroundGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ShootMeAround/Blueprints/Character/SMA_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
