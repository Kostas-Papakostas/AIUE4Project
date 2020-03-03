
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "PulseSphere_Cpp.generated.h"

/**
 * 
 */
UCLASS()
class AIPROJECT_API APulseSphere_Cpp : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APulseSphere_Cpp();

protected:
	virtual void BeginPlay() override;

	
public:

	UPROPERTY(EditAnywhere, Category = "Particles")
		UParticleSystem* pulseEmitter;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* pulseSphereMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
		class USphereComponent* KwangShield;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
		class USphereComponent* WukongOverlaps;
	
	UPROPERTY(BlueprintReadWrite, Category = "Components")
		bool NPCOverlaps;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnWukongBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
