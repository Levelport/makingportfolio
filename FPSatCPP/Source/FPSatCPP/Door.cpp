// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/BoxComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(SceneRoot);

	DoorBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBlocker"));
	DoorBlocker->SetupAttachment(DoorPivot);
	DoorBlocker->SetBoxExtent(FVector(4.f, 50.f, 100.f));
	DoorBlocker->SetRelativeLocation(FVector(0.f, 50.f, 100.f));
	DoorBlocker->SetHiddenInGame(true);
	DoorBlocker->SetCollisionProfileName(FName("BlockAllDynamic"));
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRelativeRotation = DoorPivot->GetRelativeRotation();

	if (!bHasReceivedCommand && bStartOpen)
	{
		SetOpen(true);
	}
	else if (bOpen)
	{
		SetActorTickEnabled(true);
	}
}

void ADoor::OnSwitchCommand_Implementation(EFloorSwitchMode Mode, bool bPressed)
{
	if (!bPressed || Mode == EFloorSwitchMode::TurnOff)
	{
		SetOpen(false);
		return;
	}

	if (Mode == EFloorSwitchMode::Toggle)
	{
		ToggleOpen();
		return;
	}

	SetOpen(true);
}

void ADoor::SetOpen(bool bNewOpen)
{
	bHasReceivedCommand = true;
	bOpen = bNewOpen;
	SetActorTickEnabled(true);
}

void ADoor::ToggleOpen()
{
	SetOpen(!bOpen);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FRotator TargetRotation = bOpen
		? ClosedRelativeRotation + FRotator(0.f, OpenAngle, 0.f)
		: ClosedRelativeRotation;

	const FRotator NewRotation = FMath::RInterpConstantTo(
		DoorPivot->GetRelativeRotation(),
		TargetRotation,
		DeltaTime,
		OpenSpeed);

	DoorPivot->SetRelativeRotation(NewRotation);

	if (NewRotation.Equals(TargetRotation, 0.1f))
	{
		DoorPivot->SetRelativeRotation(TargetRotation);
		SetActorTickEnabled(false);
	}
}
