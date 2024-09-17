// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WProtagWizard.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;



UCLASS()
class WIZARDRUSH_API AWProtagWizard : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandle_PrimaryAttack;



public:
	// Sets default values for this character's properties
	AWProtagWizard();

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	

	
	void PrimaryAttack_TimeElapsed();
    void PrimaryAttack();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	//Movement Functions
	void MoveForward(float  Value);
	void MoveRight(float  Value);

	// Aiming control functions
	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void StartAiming();  // Function to start aiming
	
	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void StopAiming();   // Function to stop aiming
	
	// Blueprint getter function to check if aiming
    UFUNCTION(BlueprintCallable, Category = "Aiming")
    bool IsAiming() const { return bIsAiming; }


	// Calculate the new yaw rotation based on mouse position
	void RotateCharacterToMouseCursor();

	private:
	// Flag to indicate if the character is currently aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	
	// Cached direction from character to mouse position
    FVector CachedMouseDirection; 
    
    // Function to handle starting and stopping the repeating attack
    void StartRepeatingAttack();
    void StopRepeatingAttack();
    
    // Functions For Dash Mechanic   
    void DashCharge();
    void DashInitiate();
    
private:
    float DashChargeTime;  // To track how long the dash input is held
    const float MaxDashDistance = 5000.0f;  // Maximum distance to dash
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dashing", meta = (AllowPrivateAccess = "true"))
    bool bIsDashing;

	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
