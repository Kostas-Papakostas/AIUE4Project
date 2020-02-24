// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "WukongCharracter_Cpp.generated.h"

UCLASS()
class AIPROJECT_API AWukongCharracter_Cpp : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="Mesh")
	class USkeletalMeshComponent* WukongMesh;

public:
	// Sets default values for this character's properties
	AWukongCharracter_Cpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	USkeletalMeshComponent* getSkeletalMesh();
	
	
};
