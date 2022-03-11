// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiPlayerFPS/Weapon/Gun.h"

// Sets default values
AFPSBaseCharacter::AFPSBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

// Called when the game starts or when spawned
void AFPSBaseCharacter::BeginPlay()
{
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

	/*if(InputComponent != nullptr)
	{
		//InputComponent->BindAction("Fire", IE_Pressed, this, &AFPSBaseCharacter::PullTriger);
	}*/
}

// Called every frame
void AFPSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSBaseCharacter::Server_PullTriger);

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

void AFPSBaseCharacter::Server_PullTriger_Implementation()
{
	Gun->OnFire();
}

bool AFPSBaseCharacter::Server_PullTriger_Validate()
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




