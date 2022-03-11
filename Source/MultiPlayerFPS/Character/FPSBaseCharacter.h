// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSBaseCharacter.generated.h"

UCLASS()
class MULTIPLAYERFPS_API AFPSBaseCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AFPSBaseCharacter();
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<class AGun> GunBlueprint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category="Setup")
	bool JumpButtonDown;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void UnPossessed() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class USkeletalMeshComponent* Mesh1P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = true))
	class UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = true))
	class AGun* Gun;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	void Jumping();
	void NotJumping();

public:
	/*
	 *Section For RPC
	 */

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PullTriger();

	void Local_PullTrigger();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlayAnimationAny();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_PlayAnimationAny();

	void PlayAnimationAny();
	
};
