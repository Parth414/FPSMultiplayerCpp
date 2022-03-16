// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiPlayerFPS/Weapon/Gun.h"
#include "MultiPlayerFPS/Weapon/MultiPlayerFPSProjectile.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

// Sets default values
AFPSBaseCharacter::AFPSBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Create Camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); //Position The Camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//Create Mesh that will be visible when controlling the character locally(1st person view)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocatioion = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocatioion->SetupAttachment(FP_Gun);
	FP_MuzzleLocatioion->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

}

// Called when the game starts or when spawned
void AFPSBaseCharacter::BeginPlay()
{
	GetWorld()->GetTimerManager().SetTimer(loopTimerHandle,this, &AFPSBaseCharacter::onTimerEnd,5.f,false);
	Super::BeginPlay();

	if (GunBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gun Blueprint Set to null"));
		return;
	}

	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);

	if(IsLocallyControlled())
	{
		Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}
	else
	{
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	Gun->AnimInstance1P = Mesh1P->GetAnimInstance();
	Gun->AnimInstance3P = GetMesh()->GetAnimInstance();
}

void AFPSBaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSBaseCharacter, Rep_CurrentHealth);
	DOREPLIFETIME(AFPSBaseCharacter, Rep_IsDead);
}

// Called every frame
void AFPSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DrawDebugString(GetWorld(), FVector(0,0,100), FString().Printf(TEXT("Current HP -> %0.f"), Rep_CurrentHealth), this, FColor::Green, DeltaTime);
}

// Called to bind functionality to input
void AFPSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSBaseCharacter::Jumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSBaseCharacter::NotJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSBaseCharacter::Local_PullTrigger);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSBaseCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

}

void AFPSBaseCharacter::UnPossessed()
{
	Super::UnPossessed();
	
}

void AFPSBaseCharacter::Reload()
{
	if (Gun->clipAmmo != Gun->maxClipAmmo)
	{
		if (Gun->totalAmmo - (Gun->maxClipAmmo - Gun->clipAmmo) >= 0)
		{
			Gun->totalAmmo -= (Gun->maxClipAmmo - Gun->clipAmmo);
			Gun->clipAmmo = Gun->maxClipAmmo;
		}
		else
		{
			Gun->clipAmmo += Gun->totalAmmo;
			Gun->totalAmmo = 0;
		}
	}
}

void AFPSBaseCharacter::Local_PullTrigger()
{
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		if (Gun->clipAmmo > 0)
		{
			Server_PullTrigger();
			Server_PlayAnimationAny();
			Gun->PlayAnimationOnly();
		}
	}
	else
	{
		OnFire();
		//Gun->OnFire();
		if (Gun->clipAmmo > 0)
		{
			MultiCast_PlayAnimationAny();
		}
	}
}

void AFPSBaseCharacter::PlayAnimationAny()
{
	GetMesh()->GetAnimInstance()->Montage_Play(Gun->FireAnimation3P);
	Mesh1P->GetAnimInstance()->Montage_Play(Gun->FireAnimation1P);
}

void AFPSBaseCharacter::OnFire()
{
	if (Gun->clipAmmo >= 1)
	{
		// try and fire a projectile
		if (ProjectileClass != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				FHitResult Outhit;
				FVector Start = FP_MuzzleLocatioion->GetComponentLocation();
				FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
				FVector end = ((ForwardVector * 2000.f) + Start);
				FCollisionQueryParams CollisionParams;

				DrawDebugLine(GetWorld(), Start, end, FColor::Red, true);
				Gun->PlayAnimationOnly();
				/*const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocatioion != nullptr) ? FP_MuzzleLocatioion->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMultiPlayerFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				*/

			}

		}
		Gun->clipAmmo -= 1;
	}
	else if (Gun->totalAmmo > 0)
	{
		Reload();
	}
	
	/*/else if (Gun->clipAmmo == 0)
	{

	}*/
}

void AFPSBaseCharacter::MultiCast_PlayAnimationAny_Implementation()
{
	PlayAnimationAny();
}

void AFPSBaseCharacter::Server_PlayAnimationAny_Implementation()
{
	MultiCast_PlayAnimationAny();
}

bool AFPSBaseCharacter::Server_PlayAnimationAny_Validate()
{
	return true;
}

void AFPSBaseCharacter::Server_PullTrigger_Implementation()
{
	OnFire();
	//Gun->OnFire();
}

bool AFPSBaseCharacter::Server_PullTrigger_Validate()
{
	return true;
}


void AFPSBaseCharacter::MoveForward(float Val)
{
	if(Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void AFPSBaseCharacter::MoveRight(float Val)
{
	if(Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void AFPSBaseCharacter::Jumping()
{
	Jump();
	JumpButtonDown = CanJump();
	UCharacterMovementComponent* MovComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if(MovComp->Velocity.Size() > 0)
	{
		MovComp->JumpZVelocity = 400;
	}
	else
	{
		MovComp->JumpZVelocity = 400;
	}
	
}

void AFPSBaseCharacter::NotJumping()
{
	StopJumping();
	JumpButtonDown = false;
	UCharacterMovementComponent* MovComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if(MovComp->Velocity.Size() > 0)
	{
		MovComp->JumpZVelocity = 400;
	}
	else
	{
		MovComp->JumpZVelocity = 400;
	}
}

void AFPSBaseCharacter::TakeDamageCustom_Implementation(float BaseDamage)
{
	Rep_CurrentHealth += -trunc(BaseDamage);

	if (Rep_CurrentHealth <= 0)
	{
		Rep_IsDead = true;
	}
}

void AFPSBaseCharacter::onTimerEnd()
{
	GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Yellow, "Timer is up");
}
