// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;


UCLASS()
class WIZARDRUSH_API AWProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
    USphereComponent* SphereComp;
    
   	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
   	UProjectileMovementComponent* MovementComp;
    
   	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
   	UParticleSystemComponent* EffectComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
