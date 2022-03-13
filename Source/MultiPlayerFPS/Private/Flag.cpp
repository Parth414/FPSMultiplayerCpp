// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"

// Sets default values
AFlag::AFlag()
{
	FRotator Rotation;
	Rotation.Yaw = -90.0f;
	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Flag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLAG"));
	Flag->SetActorRotation(FRotator(0.0f, 0.0f, 90.0f));

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

