// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSBaseCharacter.generated.h"

UCLASS()
class MULTIPLAYERFPS_API AFPSBaseCharacter : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, Category = "Mesh")
		class USkeletalMeshComponent* Mesh1P;

		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* FP_MuzzleLocatioion;


public:
	// Sets default values for this character's properties
	AFPSBaseCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = true))
	class UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = true))
	class AGun* Gun;

protected:
	void OnFire();
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


	UFUNCTION(BlueprintCallable, Category = "Input")
		void Reload();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PullTrigger();

	void Local_PullTrigger();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlayAnimationAny();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_PlayAnimationAny();

	void PlayAnimationAny();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TakeDamageCustom(float BaseDamage);
	
	

	/*
	 * Replicated Properties
	 */
private:
	UPROPERTY(Replicated)
	float Rep_CurrentHealth =  20;

	FTimerHandle loopTimerHandle;
	
public:
	UFUNCTION()
	void onTimerEnd();
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMultiPlayerFPSProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool Rep_IsDead = false;
};
