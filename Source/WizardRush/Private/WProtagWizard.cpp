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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	bIsAiming = false;  // Initialize the aiming flag to false
	bIsDashing = false; // Initialize the aiming flag to false
	

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
	
	// Set the character's speed to 300 while aiming
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	
	StartRepeatingAttack();  // Start the repeating attack
}

// Function to start the repeating attack
void AWProtagWizard::StartRepeatingAttack()
{
    // Start a looping timer that calls PrimaryAttack_TimeElapsed every 0.5 seconds
    GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AWProtagWizard::PrimaryAttack_TimeElapsed, 0.2f, true);
}


void AWProtagWizard::StopAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;  // Enable rotation to movement
	bUseControllerRotationYaw = false;  // Disable controller rotation when not aiming
	
	// Revert the character's speed back to 600 when not aiming
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	
	StopRepeatingAttack();  // Stop the repeating attack
}

// Function to stop the repeating attack
void AWProtagWizard::StopRepeatingAttack()
{
    // Clear the repeating attack timer
    GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
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
			//UE_LOG(LogTemp, Warning, TEXT("Projected Mouse Location: X = %f, Y = %f, Z = %f"), ProjectedMouseLocation.X, ProjectedMouseLocation.Y, ProjectedMouseLocation.Z);

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

	//PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AWProtagWizard::PrimaryAttack_TimeElapsed, 0.2f);

	//potential to clear timer or interupt animation if charachter dies
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);

}

void  AWProtagWizard:: PrimaryAttack_TimeElapsed()
{

	FVector HandLocation = GetMesh()->GetSocketLocation("hand_r");
	
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

void AWProtagWizard::DashCharge()
{
    bIsDashing = true;  // Start dash state
     
    // Stop movement but allow rotation
    GetCharacterMovement()->MaxWalkSpeed = 0.0f;
   // GetCharacterMovement()->bOrientRotationToMovement = false; // Disable rotation to movement
    //bUseControllerRotationYaw = true; // Allow rotation using controller input
    
    // Increase the rotation rate for faster rotation during dash
     GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);  // Higher value for faster rotation

    // Disable aiming and rotation to mouse
    if (bIsAiming)
    {
        StopAiming();
    }
    
    // Prevent RotateCharacterToMouseCursor from being called
    bIsAiming = false;
}
void AWProtagWizard::DashInitiate()
{
    bIsDashing = false;  // End dashCharge state
    // Calculate teleport distance based on DashChargeTime
    float TeleportDistanceMultiplier = 1500.0f; // Adjust this to control how fast teleport distance increases
    float TeleportDistance = FMath::Clamp(DashChargeTime * TeleportDistanceMultiplier, 1000.0f, MaxDashDistance);  // Adjust min and max values as needed
    
    // Determine the teleport target location
    FVector ForwardDirection = GetActorForwardVector();
    FVector StartLocation = GetActorLocation();
    FVector TargetLocation = StartLocation + (ForwardDirection * TeleportDistance);
    
    // Perform the teleport by setting the character's location
    SetActorLocation(TargetLocation, true);  // The 'true' parameter ensures that collisions are handled
    
    // Reset DashChargeTime after teleporting
     DashChargeTime = 0.0f;
     
     
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;  // Reset speed
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); //Lower Rotation Speed
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
	
    // Update DashChargeTime if dashing
    if (bIsDashing)
     {
        DashChargeTime += DeltaTime;  // Increment the charge time
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

	// Bind the DashCharge and DashInitiate functions to the input
    PlayerInputComponent->BindAction("DashCharge", IE_Pressed, this, &AWProtagWizard::DashCharge);
    PlayerInputComponent->BindAction("DashCharge", IE_Released, this, &AWProtagWizard::DashInitiate);

	

	
	
}

