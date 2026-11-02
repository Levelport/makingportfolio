// Fill out your copyright notice in the Description page of Project Settings.


#include "switchJunction.h"
#include "junctionSignInterface.h"

// Sets default values
AswitchJunction::AswitchJunction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AswitchJunction::BeginPlay()
{
	Super::BeginPlay();
	RefreshOutput();
}

void AswitchJunction::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BroadcastCount(CurrentOnCount());
}

#if WITH_EDITOR
void AswitchJunction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	BroadcastCount(CurrentOnCount());
}
#endif

// Called every frame
void AswitchJunction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AswitchJunction::OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed)
{
	if (!IsValid(Source))
	{
		return;
	}

	bool& bOn = SourceOn.FindOrAdd(TWeakObjectPtr<AActor>(Source));

	if (Mode == EFloorSwitchMode::WhilePressed)
	{
		bOn = bPressed;
	}
	else if (bPressed && Mode == EFloorSwitchMode::Toggle)
	{
		bOn = !bOn;
	}
	else if (bPressed && Mode == EFloorSwitchMode::StayOn)
	{
		bOn = true;
	}
	else if (Mode == EFloorSwitchMode::TurnOff)
	{
		bOn = false;
	}

	RefreshOutput();
}

void AswitchJunction::RefreshOutput()
{
	int32 OnCount = 0;

	for (auto It = SourceOn.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		if (It.Value())
		{
			++OnCount;
		}
	}

	BroadcastCount(OnCount);

	const bool bShouldBeOn = OnCount >= FMath::Max(1, RequiredCount);
	if (bShouldBeOn == bOutputOn)
	{
		return;
	}

	bOutputOn = bShouldBeOn;

	for (AActor* Target : Targets)
	{
		if (IsValid(Target) && Target->GetClass()->ImplementsInterface(USwitchInterface::StaticClass()))
		{
			ISwitchInterface::Execute_OnSwitchCommand(Target, this, EFloorSwitchMode::WhilePressed, bOutputOn);
		}
	}
}

int32 AswitchJunction::CurrentOnCount() const
{
	int32 OnCount = 0;
	for (const TPair<TWeakObjectPtr<AActor>, bool>& Pair : SourceOn)
	{
		if (Pair.Key.IsValid() && Pair.Value)
		{
			++OnCount;
		}
	}
	return OnCount;
}

void AswitchJunction::BroadcastCount(int32 OnCount)
{
	for (AActor* Sign : Signs)
	{
		if (IsValid(Sign) && Sign->GetClass()->ImplementsInterface(UjunctionSignInterface::StaticClass()))
		{
			IjunctionSignInterface::Execute_OnJunctionCount(Sign, OnCount, RequiredCount);
		}
	}
}
