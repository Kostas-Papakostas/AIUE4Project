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

	UPROPERTY()
		FVector SpawnLocation;

public:

	void setSpawnLocation(FVector spawnVector) { SpawnLocation = spawnVector; };
	FVector getSpawnLocation(FVector spawnVector) { return SpawnLocation; };

	/*DON'T NEED THEM*/
	UPROPERTY(EditAnywhere, Category="Components")
	class USphereComponent* Left;

	UPROPERTY(EditAnywhere, Category="Components")
	class USphereComponent* Right;
	/*DON'T NEED THEM END*/
	
	UPROPERTY(EditAnywhere,Category = "Mesh")
	class USkeletalMeshComponent* CloneMesh;

	UPROPERTY(EditAnywhere, Category="Particles Effect")
	class UParticleSystem* HitEmitter;

	UPROPERTY(EditAnywhere, Category = "Particles Effect")
	class UParticleSystem* SpawningEmitter;

	UPROPERTY(EditAnywhere, Category ="Particles Effect")
	class UParticleSystem* DespawningEmitter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation;

	UFUNCTION()
		void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

};

