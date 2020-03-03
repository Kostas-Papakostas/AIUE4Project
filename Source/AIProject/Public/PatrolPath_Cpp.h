
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath_Cpp.generated.h"

UCLASS()
class AIPROJECT_API APatrolPath_Cpp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath_Cpp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		FVector PathPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		bool IsLooping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		float WaitTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
