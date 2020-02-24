#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "WukongClone_Cpp.generated.h"

UCLASS()
class AIPROJECT_API AWukongClone_Cpp : public ACharacter
{
	GENERATED_BODY()


public:
	AWukongClone_Cpp();

protected:
	virtual void BeginPlay() override;

	AActor* actorToFollow;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	USphereComponent* AWukongClone_Cpp::getRight();
	

private:

	//UPROPERTY(BluprintReadWrite, Category = "Variable")
	UPROPERTY()
		FVector SpawnLocation;

public:

	UPROPERTY(EditAnywhere, Category="Components")
	class USphereComponent* Left;

	UPROPERTY(EditAnywhere, Category="Components")
	class USphereComponent* Right;
	
	UPROPERTY(EditAnywhere,Category = "Mesh")
	class USkeletalMeshComponent* CloneMesh;

	UPROPERTY(EditAnywhere, Category="Variable")
	class UParticleSystem* HitEmitter;

	UPROPERTY(EditAnywhere, Category = "Variable")
	class UParticleSystem* SpawningEmitter;

	UPROPERTY(EditAnywhere, Category ="Variable")
	class UParticleSystem* DespawningEmitter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation;

	UFUNCTION()
		void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

};

