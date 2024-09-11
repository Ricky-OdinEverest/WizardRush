// Fill out your copyright notice in the Description page of Project Settings.


#include "WProtagWizard.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AWProtagWizard::AWProtagWizard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->SetupAttachment(RootComponent);
	

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComp->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bIsAiming = false;  // Initialize the aiming flag to false
	

}

// Called when the game starts or when spawned
void AWProtagWizard::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = true;  // Show mouse cursor
	}
	
}


void AWProtagWizard::MoveForward(float Value)
{
    FRotator ControlRot = GetControlRotation();
    ControlRot.Pitch = 0.0f;
    ControlRot.Yaw = 0.0f;
	AddMovementInput(ControlRot.Vector(), Value);
}

void AWProtagWizard::MoveRight(float Value)
{

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
    ControlRot.Yaw = 0.0f;
	
	
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, Value);
}

// Create the Aim functions
void AWProtagWizard::StartAiming()
{
	bIsAiming = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;  // Disable rotation to movement
	bUseControllerRotationYaw = true;  // Use controller rotation when aiming
}

void AWProtagWizard::StopAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;  // Enable rotation to movement
	bUseControllerRotationYaw = false;  // Disable controller rotation when not aiming
}

// Calculate the new rotation to face the mouse cursor
void AWProtagWizard::RotateCharacterToMouseCursor()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		FVector MouseLocation, MouseDirection;
		if (PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
		{
			// Get the character's location
			FVector CharacterLocation = GetActorLocation();
            
			// Calculate the intersection point of the mouse ray and a plane at the character's Z level
			float ZPlane = CharacterLocation.Z;
			FVector PlanePoint = FVector(MouseLocation.X, MouseLocation.Y, ZPlane);
            
			// Calculate the intersection point by projecting the mouse direction to the plane at ZPlane
			float T = (ZPlane - MouseLocation.Z) / MouseDirection.Z;
			FVector ProjectedMouseLocation = MouseLocation + T * MouseDirection;

			// Debug: Print the projected mouse location to the log
			UE_LOG(LogTemp, Warning, TEXT("Projected Mouse Location: X = %f, Y = %f, Z = %f"), ProjectedMouseLocation.X, ProjectedMouseLocation.Y, ProjectedMouseLocation.Z);

			// Calculate direction to the projected mouse location
			FVector DirectionToMouse = (ProjectedMouseLocation - CharacterLocation).GetSafeNormal();

			// Create a new yaw rotation from the direction
			FRotator TargetRotation = FRotator(0.0f, DirectionToMouse.Rotation().Yaw, 0.0f);

			// Set the character's rotation directly to face the mouse
			SetActorRotation(TargetRotation);
			
			 // Store the calculated mouse direction to use later in firing
             CachedMouseDirection = DirectionToMouse;
		}
	}
}

void AWProtagWizard::PrimaryAttack()
{

	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AWProtagWizard::PrimaryAttack_TimeElapsed, 0.2f);

	//potential to clear timer or interupt animation if charachter dies
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);

}

void  AWProtagWizard:: PrimaryAttack_TimeElapsed()
{

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	
	// Use the cached mouse direction calculated in RotateCharacterToMouseCursor
    FVector AimDirection = CachedMouseDirection;
     
    // Calculate rotation from aim direction
    FRotator AimRotation = FRotationMatrix::MakeFromX(AimDirection).Rotator();
    
     FTransform SpawnTM = FTransform(AimRotation, HandLocation);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Instigator = this;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

	
	




}



// Called every frame
void AWProtagWizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Rotate character to face mouse when aiming
	if (bIsAiming)
	{
		RotateCharacterToMouseCursor();
	}

}



// Called to bind functionality to input
void AWProtagWizard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWProtagWizard::MoveForward);

	PlayerInputComponent->BindAxis("MoveRight", this, &AWProtagWizard::MoveRight);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AWProtagWizard::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AWProtagWizard::StopAiming);
	
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AWProtagWizard::PrimaryAttack);

	

	

	
	
}

