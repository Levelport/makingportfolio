// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockDispencer.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABlockDispencer::ABlockDispencer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(SceneRoot);
	Body->SetMobility(EComponentMobility::Movable);
	Body->SetSimulatePhysics(false);
	Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Body->SetCollisionResponseToAllChannels(ECR_Block);
	Body->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.3f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (BodyMesh.Succeeded())
	{
		Body->SetStaticMesh(BodyMesh.Object);
	}

	DropPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DropPoint"));
	DropPoint->SetupAttachment(SceneRoot);
	DropPoint->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
}

// Called when the game starts or when spawned
void ABlockDispencer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockDispencer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockDispencer::OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed)
{
	if (!bPressed)
	{
		return;
	}

	DropCube();
}

void ABlockDispencer::DropCube()
{
	UClass* Class = CubeClass.LoadSynchronous();
	UWorld* World = GetWorld();
	if (!Class || !World || !DropPoint)
	{
		return;
	}

	if (IsValid(SpawnedCube))
	{
		SpawnedCube->SetActorEnableCollision(false);
		SpawnedCube->SetActorHiddenInGame(true);
		SpawnedCube->Destroy();
		SpawnedCube = nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnedCube = World->SpawnActor<AActor>(Class, DropPoint->GetComponentTransform(), Params);
}
