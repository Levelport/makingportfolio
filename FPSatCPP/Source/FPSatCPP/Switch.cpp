// Fill out your copyright notice in the Description page of Project Settings.


#include "Switch.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	void ConfigureSwitchRoot(UBoxComponent* Bounds)
	{
		Bounds->SetHiddenInGame(true);
		Bounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Bounds->SetGenerateOverlapEvents(false);
	}

	void ConfigurePlateCollision(UStaticMeshComponent* PlateMesh)
	{
		PlateMesh->SetMobility(EComponentMobility::Movable);
		PlateMesh->SetSimulatePhysics(false);
		PlateMesh->SetUsingAbsoluteLocation(false);
		PlateMesh->SetUsingAbsoluteRotation(false);
		PlateMesh->SetUsingAbsoluteScale(false);
		PlateMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PlateMesh->SetCollisionObjectType(ECC_WorldDynamic);
		PlateMesh->SetCollisionResponseToAllChannels(ECR_Block);
		PlateMesh->SetGenerateOverlapEvents(false);
		PlateMesh->CanCharacterStepUpOn = ECB_Yes;
	}

	void ConfigurePlateTrigger(UBoxComponent* Trigger)
	{
		Trigger->SetHiddenInGame(true);
		Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
		Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Trigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
		Trigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Trigger->SetGenerateOverlapEvents(true);
	}
}

// Sets default values
ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBounds"));
	SetRootComponent(InteractBounds);
	InteractBounds->SetBoxExtent(FVector(70.f, 70.f, 80.f));
	ConfigureSwitchRoot(InteractBounds);

	Plate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plate"));
	Plate->SetupAttachment(InteractBounds);
	Plate->SetCastShadow(false);
	ConfigurePlateCollision(Plate);

	PlateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PlateTrigger"));
	PlateTrigger->SetupAttachment(InteractBounds);
	ConfigurePlateTrigger(PlateTrigger);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlateMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (PlateMesh.Succeeded())
	{
		Plate->SetStaticMesh(PlateMesh.Object);
	}
}

void ASwitch::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	PlacePlate();
	UpdatePlate();
}

// Called when the game starts or when spawned
void ASwitch::BeginPlay()
{
	Super::BeginPlay();

	ConfigureSwitchRoot(InteractBounds);
	ConfigurePlateCollision(Plate);
	ConfigurePlateTrigger(PlateTrigger);
	PlacePlate();

	PlateTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &ASwitch::OnBoundsBeginOverlap);
	PlateTrigger->OnComponentEndOverlap.RemoveDynamic(this, &ASwitch::OnBoundsEndOverlap);
	PlateTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASwitch::OnBoundsBeginOverlap);
	PlateTrigger->OnComponentEndOverlap.AddDynamic(this, &ASwitch::OnBoundsEndOverlap);
	PlateTrigger->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	PlateTrigger->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TInlineComponentArray<UPrimitiveComponent*> Components;
		Actor->GetComponents(Components);
		for (UPrimitiveComponent* Component : Components)
		{
			if (CountsAsWeight(Actor, Component))
			{
				Weights.Add(Actor);
				break;
			}
		}
	}

	UpdatePlate();

	if (NumWeights() > 0 && PressMode != EFloorSwitchMode::Toggle)
	{
		SendCommand(true);
	}
}

// Called every frame
void ASwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASwitch::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CountsAsWeight(OtherActor, OtherComp))
	{
		return;
	}

	const bool bWasEmpty = NumWeights() == 0;
	Weights.Add(OtherActor);
	UpdatePlate();

	if (!bWasEmpty)
	{
		return;
	}

	SendCommand(true);
}

void ASwitch::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Weights.Contains(OtherActor))
	{
		return;
	}

	TArray<AActor*> StillOverlapping;
	PlateTrigger->GetOverlappingActors(StillOverlapping);
	if (StillOverlapping.Contains(OtherActor))
	{
		return;
	}

	Weights.Remove(OtherActor);
	UpdatePlate();

	if (NumWeights() == 0 && PressMode == EFloorSwitchMode::WhilePressed)
	{
		SendCommand(false);
	}
}

void ASwitch::SendCommand(bool bPressed)
{
	for (AActor* Target : Targets)
	{
		if (IsValid(Target) && Target->GetClass()->ImplementsInterface(USwitchInterface::StaticClass()))
		{
			ISwitchInterface::Execute_OnSwitchCommand(Target, this, PressMode, bPressed);
		}
	}

	OnSwitched(bPressed && PressMode != EFloorSwitchMode::TurnOff);

}

bool ASwitch::CountsAsWeight(AActor* OtherActor, UPrimitiveComponent* OtherComp) const
{
	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return false;
	}

	if (Cast<APawn>(OtherActor))
	{
		return true;
	}

	return OtherComp && OtherComp->IsSimulatingPhysics();
}

int32 ASwitch::NumWeights() const
{
	int32 Count = 0;
	for (const TWeakObjectPtr<AActor>& Weight : Weights)
	{
		if (Weight.IsValid())
		{
			++Count;
		}
	}
	return Count;
}

void ASwitch::PlacePlate()
{
	if (!Plate || !InteractBounds)
	{
		return;
	}

	Plate->SetUsingAbsoluteLocation(false);
	Plate->SetUsingAbsoluteRotation(false);
	Plate->SetUsingAbsoluteScale(false);
	if (Plate->GetAttachParent() != InteractBounds)
	{
		Plate->AttachToComponent(InteractBounds, FAttachmentTransformRules::KeepRelativeTransform);
	}

	const FVector Extent = InteractBounds->GetUnscaledBoxExtent();
	const float Thickness = 10.f;
	const float HalfX = Extent.X * 0.9f;
	const float HalfY = Extent.Y * 0.9f;
	Plate->SetRelativeScale3D(FVector(HalfX * 2.f / 100.f, HalfY * 2.f / 100.f, Thickness / 100.f));
	Plate->SetRelativeLocation(FVector(0.f, 0.f, -Extent.Z + Thickness * 0.5f));
	Plate->SetRelativeRotation(FRotator::ZeroRotator);

	if (PlateTrigger)
	{
		PlateTrigger->SetUsingAbsoluteLocation(false);
		PlateTrigger->SetUsingAbsoluteRotation(false);
		PlateTrigger->SetUsingAbsoluteScale(false);
		if (PlateTrigger->GetAttachParent() != InteractBounds)
		{
			PlateTrigger->AttachToComponent(InteractBounds, FAttachmentTransformRules::KeepRelativeTransform);
		}

		const float TriggerHeight = 36.f;
		const float PlateTopZ = -Extent.Z + Thickness;
		PlateTrigger->SetBoxExtent(FVector(HalfX, HalfY, TriggerHeight * 0.5f));
		PlateTrigger->SetRelativeLocation(FVector(0.f, 0.f, PlateTopZ + TriggerHeight * 0.5f + 1.f));
		PlateTrigger->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void ASwitch::UpdatePlate()
{
	PlacePlate();

	if (!Plate)
	{
		return;
	}

	if (!PlateMaterial)
	{
		if (UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
		{
			PlateMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			Plate->SetMaterial(0, PlateMaterial);
		}
	}

	if (!PlateMaterial)
	{
		return;
	}

	const FLinearColor Color = NumWeights() > 0 ? OnColor : OffColor;
	PlateMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	PlateMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
}
