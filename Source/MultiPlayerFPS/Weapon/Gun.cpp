// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "Kismet/GameplayStatics.h"
#include "MultiPlayerFPSProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//Set Ammo Values
	maxTotalAmmo = 100;
	maxClipAmmo = 12;
	totalAmmo = 64;
	clipAmmo = 12;
	reloadTime = 1;

}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::OnFire()
{
	/*if (clipAmmo > 0)
	{*/
		// try and fire a projectile
		if (ProjectileClass != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				FHitResult Outhit;
				FVector Start = FP_MuzzleLocation->GetComponentLocation();
				FVector ForwardVector = FP_MuzzleLocation->GetForwardVector();
				FVector end = ((ForwardVector * 2000.f) + Start);
				FCollisionQueryParams CollisionParams;

				DrawDebugLine(GetWorld(), Start, end, FColor::Red, true);

				const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMultiPlayerFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				
			}
			
		}/*
		clipAmmo -= 1;
	}
	else if (totalAmmo > 0)
	{
		Reload();
	}*/
	PlayAnimationOnly();
}

void AGun::Reload()
{
	if (clipAmmo != maxClipAmmo)
	{
		if (totalAmmo - (maxClipAmmo - clipAmmo) >= 0)
		{
			totalAmmo -= (maxClipAmmo - clipAmmo);
			clipAmmo = maxClipAmmo;
		}
		else
		{
			clipAmmo += totalAmmo;
			totalAmmo = 0;
		}
	}
}

void AGun::OnFireAnimationOnly()
{

	switch (GetLocalRole())
	{
	case ENetRole::ROLE_AutonomousProxy:
		
		break;
	case ENetRole::ROLE_Authority:
		break;
	case ENetRole::ROLE_SimulatedProxy:
		break;
	}
}

void AGun::PlayAnimationOnly()
{
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified & Get the animation object for the arms mesh
	if (FireAnimation1P != nullptr && AnimInstance1P != nullptr)
	{
		AnimInstance1P->Montage_Play(FireAnimation1P, 1.f);
	}
}


