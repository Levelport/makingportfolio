// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ATestMyCharacter::ATestMyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
}
