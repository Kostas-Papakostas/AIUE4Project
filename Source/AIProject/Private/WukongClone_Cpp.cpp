#include "WukongClone_Cpp.h"
#include "NPC_Cpp.h"
#include "animationInstance.h"
#include "Animation/AnimInstance.h"
#include "Delegates/Delegate.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AWukongClone_Cpp::AWukongClone_Cpp()
{
	PrimaryActorTick.bCanEverTick = true;
	/*set clone mesh and attach it to capsule*/
	CloneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CloneMeshComponent"));
	CloneMesh->SetOnlyOwnerSee(true);
	CloneMesh->SetupAttachment(GetCapsuleComponent());
	CloneMesh->CastShadow = true;
	CloneMesh->bCastDynamicShadow = true;

	//CloneMesh->RelativeRotation = FRotator{ 0,-90.f,0 };
	//CloneMesh->RelativeLocation = FVector{ 0,0,-90.f };
	CloneMesh->SetRelativeLocation(FVector{ 0,0,-90.f });
	CloneMesh->SetRelativeRotation(FRotator{ 0,-90.f,0 });


	Right = CreateDefaultSubobject<USphereComponent>(TEXT("Right sphere"));
	Right->InitSphereRadius(105.f);
	Right->SetupAttachment(GetCapsuleComponent());

	Left = CreateDefaultSubobject<USphereComponent>(TEXT("Left sphere"));
	Left->InitSphereRadius(105.f);
	Left->SetupAttachment(GetCapsuleComponent());
}


void AWukongClone_Cpp::BeginPlay() {
	Super::BeginPlay();

	/*spawn emitters when a clone spawned*/
	FVector socketUpVector=UKismetMathLibrary::GetUpVector(CloneMesh->GetSocketRotation("FX_Head"))*50.f;
	SpawnLocation=socketUpVector+ CloneMesh->GetSocketLocation("FX_Head");
	FTransform emitterTransform{ FRotator::ZeroRotator,SpawnLocation,FVector::OneVector };
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawningEmitter, emitterTransform,true,EPSCPoolMethod::None);

}

void AWukongClone_Cpp::Destroyed() {
	Super::Destroyed();

	/*spawn emitters when clones get destroyed*/
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DespawningEmitter, { FRotator::ZeroRotator, CloneMesh->GetSocketLocation("FX_Root"), FVector::OneVector });
}

void AWukongClone_Cpp::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	TArray<AActor*> NPCActors;

	/*if clone still exists rotates towards enemy entered the ulti field*/
	if (!IsActorBeingDestroyed()) {
		UGameplayStatics::GetAllActorsOfClass(this, ANPC_Cpp::StaticClass(), NPCActors);
		FVector targetLocation = dynamic_cast<ACharacter*>(NPCActors.operator[](0))->GetCapsuleComponent()->GetComponentLocation();
		actorToFollow = NPCActors.operator[](0);
		FTransform currentTransform = GetCapsuleComponent()->GetComponentTransform();
		
		FRotator worldRotation = UKismetMathLibrary::RInterpTo(currentTransform.GetRotation().Rotator(), 
			UKismetMathLibrary::FindLookAtRotation(currentTransform.GetLocation(), targetLocation), DeltaTime, 15.f);
		
		GetCapsuleComponent()->SetWorldRotation(worldRotation.Quaternion(), false, nullptr, ETeleportType::None);
		UKismetSystemLibrary::Delay(this, 1.f, FLatentActionInfo::FLatentActionInfo());

		/*checkes the distance of enemy and itself to perform an attack*/
		if (GetDistanceTo(NPCActors.operator[](0)) <= 180) {
			if (hitAnimation != NULL) {
				UAnimInstance *tempInstance = CloneMesh->GetAnimInstance();
				if (tempInstance != NULL) {
					tempInstance->Montage_Play(hitAnimation, 1.f);
					tempInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongClone_Cpp::OnNotifyBegin);
				}
			}
		}
	}

}

/*don't need that*/
void AWukongClone_Cpp::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

USphereComponent* AWukongClone_Cpp::getRight()
{
	return Right;
}

/*animation notify begin event*/
void AWukongClone_Cpp::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (!NotifyName.ToString().IsEmpty()) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, { FRotator::ZeroRotator,actorToFollow->GetActorLocation(),FVector::OneVector });
		UGameplayStatics::ApplyDamage(actorToFollow, 250.f, NULL, this, NULL);
	}
}