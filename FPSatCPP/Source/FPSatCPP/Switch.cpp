// Fill out your copyright notice in the Description page of Project Settings.


#include "Switch.h"
#include "Components/BoxComponent.h"
#include "Door.h"
#include "GameFramework/Pawn.h"

// Sets default values
ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBounds"));
	SetRootComponent(InteractBounds);

	InteractBounds->SetBoxExtent(FVector(70.f, 70.f, 80.f));
	InteractBounds->SetHiddenInGame(true);
	InteractBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractBounds->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ASwitch::BeginPlay()
{
	Super::BeginPlay();

	InteractBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractBounds->SetGenerateOverlapEvents(true);
	InteractBounds->SetBoxExtent(FVector(70.f, 70.f, 80.f));

	InteractBounds->OnComponentBeginOverlap.AddDynamic(this, &ASwitch::OnBoundsBeginOverlap);
	InteractBounds->OnComponentEndOverlap.AddDynamic(this, &ASwitch::OnBoundsEndOverlap);
	InteractBounds->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	InteractBounds->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Cast<APawn>(Actor))
		{
			++OverlappingPawns;
		}
	}

	if (OverlappingPawns > 0 && PressMode != EFloorSwitchMode::Toggle)
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
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	const bool bWasEmpty = OverlappingPawns == 0;
	++OverlappingPawns;

	if (!bWasEmpty)
	{
		return;
	}

	SendCommand(true);
}

void ASwitch::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	OverlappingPawns = FMath::Max(0, OverlappingPawns - 1);

	if (OverlappingPawns == 0 && PressMode == EFloorSwitchMode::WhilePressed)
	{
		SendCommand(false);
	}

}

void ASwitch::SendCommand(bool bPressed)
{
	for (AActor* Target : Targets)
	{
		ADoor* Door = Cast<ADoor>(Target);
		if (!Door)
		{
			continue;
		}

		if (!bPressed)
		{
			Door->SetOpen(false);
			continue;
		}

		switch (PressMode)
		{
		case EFloorSwitchMode::Toggle:
			Door->ToggleOpen();
			break;
		case EFloorSwitchMode::TurnOff:
			Door->SetOpen(false);
			break;
		case EFloorSwitchMode::WhilePressed:
		case EFloorSwitchMode::StayOn:
		default:
			Door->SetOpen(true);
			break;
		}
	}

	OnSwitched(bPressed && PressMode != EFloorSwitchMode::TurnOff);

}


