// Fill out your copyright notice in the Description page of Project Settings.


#include "BadFog.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABadFog::ABadFog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FogVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("FogVolume"));
	SetRootComponent(FogVolume);
	FogVolume->SetBoxExtent(FVector(300.f, 300.f, 40.f));
	FogVolume->SetHiddenInGame(true);
	FogVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FogVolume->SetCollisionObjectType(ECC_WorldDynamic);
	FogVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	FogVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FogVolume->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABadFog::BeginPlay()
{
	Super::BeginPlay();

	if (FogVolume)
	{
		FogVolume->OnComponentBeginOverlap.AddDynamic(this, &ABadFog::OnFogBeginOverlap);
	}
}

// Called every frame
void ABadFog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABadFog::OnFogBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (bRetryStarted || !Pawn || !Pawn->IsPlayerControlled() || !GetWorld())
	{
		return;
	}

	bRetryStarted = true;

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}
