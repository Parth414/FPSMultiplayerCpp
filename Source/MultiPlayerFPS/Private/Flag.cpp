// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// Sets default values
AFlag::AFlag()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	hasFlag = false;

	Flag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLAG"));
	Flag->OnComponentBeginOverlap.AddDynamic(this, &AFlag::Overlap);
	RootComponent = Flag;
	RootComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

}

// Called when the game starts or when spawned
void AFlag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlag::Overlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
	hasFlag = true;
	//Flag->Deactivate()/*SetVisibility(false)*/;
	
}


