
#include "PatrolPath_Cpp.h"


// Sets default values
APatrolPath_Cpp::APatrolPath_Cpp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WaitTime = 0.5f;
	IsLooping = false;
}

// Called when the game starts or when spawned
void APatrolPath_Cpp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatrolPath_Cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

