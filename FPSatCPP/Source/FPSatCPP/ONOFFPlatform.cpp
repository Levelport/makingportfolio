// Fill out your copyright notice in the Description page of Project Settings.


#include "ONOFFPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AONOFFPlatform::AONOFFPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	SetRootComponent(Platform);
	Platform->SetMobility(EComponentMobility::Movable);
	Platform->SetSimulatePhysics(false);
	Platform->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));
	Platform->SetCollisionProfileName(FName("BlockAllDynamic"));
	Platform->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Platform->SetStaticMesh(CubeMesh.Object);
	}
}

void AONOFFPlatform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyFromSignal();
	bHasReceivedCommand = false;
}

// Called when the game starts or when spawned
void AONOFFPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (!bHasReceivedCommand)
	{
		ApplyFromSignal();
	}
}

void AONOFFPlatform::OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed)
{
	DriveState.Apply(Source, Mode, bPressed);
	ApplyFromSignal();
}

void AONOFFPlatform::ApplyFromSignal()
{
	const bool bSignalOn = DriveState.IsOn();
	ApplyGone(bStartOn ? bSignalOn : !bSignalOn);
}

void AONOFFPlatform::ApplyGone(bool bNewGone)
{
	bGone = bNewGone;
	bHasReceivedCommand = true;

	if (!Platform)
	{
		return;
	}

	Platform->SetVisibility(!bGone);
	Platform->SetHiddenInGame(bGone);
	Platform->SetCollisionEnabled(bGone ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

// Called every frame
void AONOFFPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
