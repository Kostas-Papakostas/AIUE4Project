#include "WukongCharracter_Cpp.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "NPC_Cpp.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "WukongAnimEventGraph_Cpp.h"
#include "WukongClone_Cpp.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DestructibleComponent.h"
#include <math.h>

// Sets default values
AWukongCharracter_Cpp::AWukongCharracter_Cpp()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	GetCapsuleComponent()->InitCapsuleSize(86.f, 86.f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PrimaryActorTick.bCanEverTick = true;
	FlipFlopEvade = false;
	isEvading = false;
	Stunned = false;
	Recovered = true;
	UltiActive=false;
	UltiCallingAndCasting=false;
	DoubleJump=false;
	KnockedBack=false;
	SaveAttack=false;
	IsAttacking=false;
	OnHit=false;
	PokeRdy=true;
	RightClickHit=false;
	FlipFlopEvade=false; //STARTS AS FALSE, CAUSE THERE IS NO EVADE
	FlipFlopEmote=false;
	UltiRdy=true;
	Qslam=true;


	WukongMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WukongSkeletalMesh"));
	WukongMesh->SetupAttachment(GetCapsuleComponent());
	WukongMesh->SetAnimInstanceClass(UWukongAnimEventGraph_Cpp::StaticClass());

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 420.f;//need to be changed
	GetCharacterMovement()->AirControl = 0.05f;
	GetCharacterMovement()->BrakingDecelerationFalling = 50.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 1.5f;
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->MaxCustomMovementSpeed = 900.f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->ProbeChannel = ECollisionChannel::ECC_WorldStatic;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/*some initilizations*/
	p_Wukong_Trail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Wukong_trail"));
	p_Wukong_Trail->SetupAttachment(WukongMesh);

	staff_Particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("staff_Particles"));
	staff_Particles->SetupAttachment(WukongMesh);
	
	StaffTipA = CreateDefaultSubobject<UCapsuleComponent>(TEXT("StaffTipA"));
	StaffTipA->SetupAttachment(WukongMesh);

	ExtendC = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ExtendC"));
	ExtendC->SetupAttachment(WukongMesh);
	
	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point"));
	sphere->SetupAttachment(GetCapsuleComponent());
	
	sphere1 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point1"));
	sphere1->SetupAttachment(GetCapsuleComponent());
	
	sphere2 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point2"));
	sphere2->SetupAttachment(GetCapsuleComponent());

	sphere3 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point3"));
	sphere3->SetupAttachment(GetCapsuleComponent());

	sphere4 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point4"));
	sphere4->SetupAttachment(GetCapsuleComponent());

	sphere5 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point5"));
	sphere5->SetupAttachment(GetCapsuleComponent());

	sphere6 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point6"));
	sphere6->SetupAttachment(GetCapsuleComponent());

	sphere7 = CreateDefaultSubobject<USphereComponent>(TEXT("Clones_Point7"));
	sphere7->SetupAttachment(GetCapsuleComponent());

	
	spheresArray.Append(spheresList, ARRAY_COUNT(spheresList));
	StaffTipA->AttachToComponent(WukongMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("FX_Staff_Tip_A"));

	ExtendC->AttachToComponent(WukongMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("FX_Staff_Telescope_D"));

	objects.Add(EObjectTypeQuery::ObjectTypeQuery1);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery3);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery4);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery6);
}

// Called when the game starts or when spawned
void AWukongCharracter_Cpp::BeginPlay()
{
	Super::BeginPlay();
	previousLocation = this->GetActorLocation();
	UltiCooldown = UGameplayStatics::GetRealTimeSeconds(GetWorld()) + UltiCooldown;
	IgnoreActors.Add(this);
	ExtendC->SetActive(false, false);
	ExtendC->OnComponentBeginOverlap.AddDynamic(this, &AWukongCharracter_Cpp::PokeHitEvent);
}

// Called every frame
void AWukongCharracter_Cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*CHECKS IF STUN IS OVER, AND ABILLITIES' COOLDOWN*/
	if (Qslam) {
		if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= slamCooldown) {
			Qslam = false;
		}
	}
	slopeAngle = ComputeSlopeAngle();
	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= StunTimer) {
		Stunned = false;
		Recovered = true;
	}

	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= UltiCooldown) {
		UltiRdy = true;
	}
	if (UltiActive) {
		if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= UltiTimer) {//IF ULTI IS OVER, DESPAWN ALL THE CLONES
			UltiActive = false;
			TArray<AActor*> outActors_v;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("WukongClone"), outActors_v);
			for (AActor* eachA : outActors_v) {
				eachA->Destroy();
				UltiCallingAndCasting = false;
			}
		}
		else {//IF ULTI IS ACTIVE
			TArray<AActor*> outActors_v;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("WukongClone"), outActors_v);
			radius += 50.f;//NO NEED
			IgnoreActors.Append(outActors_v);//IGNORE CLONES
			FHitResult outHitStruct;
			TArray<TEnumAsByte<EObjectTypeQuery>> obj_interact;
			obj_interact.Add(EObjectTypeQuery::ObjectTypeQuery6);//PAWN

			UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetCapsuleComponent()->GetComponentLocation(), (GetCapsuleComponent()->GetUpVector() + GetCapsuleComponent()->GetComponentLocation()), 500.f, obj_interact, false, IgnoreActors, EDrawDebugTrace::None,outHitStruct,true);

			if (UKismetSystemLibrary::GetDisplayName(Cast<UObject>(outHitStruct.Actor))=="NPC_Cpp") {//CHECKS IF NPC ENTERED THE AREA, AND DO SOME STUFF
				UKismetSystemLibrary::Delay(GetWorld(), 2.f, FLatentActionInfo::FLatentActionInfo());
				ANPC_Cpp* temp = Cast<ANPC_Cpp>(outHitStruct.Actor);
				temp->StStart = true;//MINI STUNS
				temp->StunRecoveryTimer = ComputeCooldown(1.5);
				temp->GetCharacterMovement()->StopMovementImmediately();
			}
		}
	}
	/*END OF TIME CHECKING*/

	/*CHECKS IF DEAD*/
	if (HPRemaining <= 0) {
		TArray<AActor*> outActors_v;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC_Cpp::StaticClass(), outActors_v);
		Cast<ANPC_Cpp>(outActors_v.operator[](0))->playerIsDead();
		Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->Dead = true;
		GetCharacterMovement()->StopMovementImmediately();
	}
}

float AWukongCharracter_Cpp::ComputeSlopeAngle()
{
	//BEING USED IN THE BLENDSPACE IN ORDER TO CHENGE CHARRACTER'S SLOPE ANGLE
	//i.e. upstairs downstairs
	FRotator look = UKismetMathLibrary::FindLookAtRotation(previousLocation, this->GetActorLocation());
	previousLocation = previousLocation;
	
	float radiansAtan = UKismetMathLibrary::DegreesToRadians((float)atan(look.Pitch) * 180 / PI);
	return (radiansAtan*10.f);
}

/*HANDLES JUMP AND DOUBLE JUMP EVENTS*/
void AWukongCharracter_Cpp::Jump_Function() {
	if (!Stunned) {
		if (JumpCounter <= 1) {
			JumpCounter++;
			if (JumpCounter < 2) {
				Jump();
			}
			else {
				DoubleJump = true;
				if (this->GetCharacterMovement()->IsFalling()) {
					this->LaunchCharacter(FVector(0.f, 0.f, 2000.f), false, true);
					FVector location(WukongMesh->GetSocketLocation(WukongMesh->GetSocketBoneName(TEXT("root"))));

					UGameplayStatics::SpawnEmitterAtLocation(this, p_Jump_Cloud,location, FRotator::ZeroRotator, FVector::OneVector);//SPAWN THE CLOUD EFFECT

					UKismetSystemLibrary::Delay(GetWorld(), 2.f, FLatentActionInfo::FLatentActionInfo());//WAITS THE ANIMATION TO END
					DoubleJump = false;
				}
			}
		}
	}
}

//HANDLES THE LANDED EVENTS
void AWukongCharracter_Cpp::Landed(const FHitResult &Hit) {
	Super::Landed(Hit);

	StopJumping();
	JumpCounter = 0;
}

/*DEFAULT FUNCTIONS*/
void AWukongCharracter_Cpp::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AWukongCharracter_Cpp::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AWukongCharracter_Cpp::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}
/*DEFAULT FUNCTIONS END*/

void AWukongCharracter_Cpp::ComboAttackSave() {
	//KEEPS THE PREVIOUS ATTACK TO DO THE COMBO
	if (SaveAttack) {
		SaveAttack = false;
		UAnimInstance* animInstance = WukongMesh->GetAnimInstance();
		pickAMontage(AttackCount, animInstance);
	}
}

//DISTRACTS THE NPC WITH THE NOISE
void AWukongCharracter_Cpp::NoiseDistraction() {
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),s_Wukong_Distraction, this->GetActorLocation());
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), this->GetActorLocation(), 1.f,(AActor*)this, 15000.f, TEXT("Noise"));
}


void AWukongCharracter_Cpp::ResetCombo() {//IF PLAYER WON'T HIT OR REACHES THE 4th ANIMATION. RESETS THE COMBO
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
}

void AWukongCharracter_Cpp::SimpleHit() {//THIS TRIGGERS THE HIT
	if (!isEvading && !Stunned && !UltiActive && !KnockedBack) {
		if (IsAttacking) {
			SaveAttack = true;
		}
		else {
			IsAttacking = true;
			UAnimInstance* animMontage = WukongMesh->GetAnimInstance();
			pickAMontage(AttackCount, animMontage);
		}
	}
}

/*PICKS THE ANIM MONTAGE TO PLAY*/
inline void AWukongCharracter_Cpp::pickAMontage(int attaCount_p, UAnimInstance* animMontage_p) {
	switch (AttackCount) {
	case 0:
		AttackCount = 1;
		if (hitAnimation0 != NULL) {
			animMontage_p->Montage_Play(hitAnimation0, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 1:
		AttackCount = 2;
		if (hitAnimation1 != NULL) {
			animMontage_p->Montage_Play(hitAnimation1, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 2:
		AttackCount = 0;
		if (hitAnimation2 != NULL) {
			animMontage_p->Montage_Play(hitAnimation2, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 3:
		AttackCount = 4;
		if (hitAnimation3 != NULL) {
			animMontage_p->Montage_Play(hitAnimation3, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}

	}
}

//NOTIFIES WHEN CAPSULES BEGIN/END OVERLAPPING A COMPONENT
void AWukongCharracter_Cpp::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {
	OnHit = true;
}

void AWukongCharracter_Cpp::OnNotifyBeginPoke(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {
	ExtendC->SetActive(true, false);
}

void AWukongCharracter_Cpp::OnNotifyEndPoke(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {
	ExtendC->SetActive(false, false);
	RightClickHit = false;
}
//NOTIFIES END

/*HANDLES THE RIGHT CLICK HIT(COOLDOWN, MONTAGE)*/
void AWukongCharracter_Cpp::PokeHit() {
	if (!isEvading&&!UltiActive&&!Stunned&&!UltiCallingAndCasting&&PokeRdy) {
		RightClickHit = true;
		UAnimInstance* PokeMontage = WukongMesh->GetAnimInstance();
		PokeMontage->Montage_Play(hitPoke);
		IsAttacking = false;
		PokeRdy = false;
		PokeCooldownTimer = ComputeCooldown(1.5f);
		/*ADDS DYNAMIC DELEGATE FUNCITONS*/
		PokeMontage->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBeginPoke);
		PokeMontage->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyEndPoke);
	}
}

/*HANDLES THE CAPSULE EVENT*/
void AWukongCharracter_Cpp::PokeHitEvent(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FString actor_Name = UKismetSystemLibrary::GetDisplayName(OtherActor);

	if (RightClickHit) {
		{
			if (actor_Name.Compare(TEXT("NPC_Cpp"), ESearchCase::CaseSensitive)) {/*IF IT HITS NPC ADDS A STUN, AND APPLIES DAMAGE*/
				Cast<ANPC_Cpp>(OtherActor)->StStart = true;
				Cast<ANPC_Cpp>(OtherActor)->StunRecoveryTimer = ComputeCooldown(1.5);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), p_Wukong_Poke_Hit, Cast<ANPC_Cpp>(OtherActor)->getRootComponent()->GetComponentLocation());
				UGameplayStatics::ApplyDamage(Cast<ANPC_Cpp>(OtherActor), 75.f, nullptr, this, nullptr);
			}
		}

		{
			if (OtherComponent->IsSimulatingPhysics()) {/*IF OBJECT IS SIMULATING PHYSICS, DOES STUFF*/
				FVector impulse = ((OtherComponent->GetUpVector()*-1.f) + OtherComponent->GetComponentLocation())*120.f;
				if (Cast<UPrimitiveComponent>(OtherComponent) != nullptr) {
					Cast<UPrimitiveComponent>(OtherComponent)->AddImpulseAtLocation(impulse, OtherComponent->GetComponentLocation());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), p_Wukong_Poke_Hit, OtherComponent->GetComponentLocation());
				}
			}
		}
	}
}

/*HANDLES THE SIMPLE HIT COLLISION EVEMTS*/
void AWukongCharracter_Cpp::SimpleHitEvent(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	USkeletalMeshComponent* NPCmesh;
	if (OnHit) {//IF I HIT
		NPCmesh= Cast<ANPC_Cpp>(OtherActor)->KwangMesh;
		FHitResult outHit_v;
		if (OtherComponent == Cast<ANPC_Cpp>(OtherActor)->GetCapsuleComponent()) {
			FVector endPoint = UKismetMathLibrary::GetUpVector(StaffTipA->GetComponentRotation())*150.f + StaffTipA->GetComponentLocation();
			//FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic));

				UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StaffTipA->GetComponentLocation(), endPoint, 120.f,ETraceTypeQuery::TraceTypeQuery2 , false, IgnoreActors,EDrawDebugTrace::None,outHit_v,true);//CREATES A SPHERE TRACE WHEN OVERLAPPED CAPSULE

				if (UKismetSystemLibrary::GetDisplayName(Cast<UObject>(outHit_v.Actor)) == "NPC_Cpp") {
					/*IF OVERLAPS THE NPC THEN COMPUTE HIT SIDE, SPAWN EMITTERS AND APPLYDAMAGE*/
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),p_Wukong_Staff_Impact, StaffTipA->GetComponentLocation());
					Cast<ANPC_Cpp>(OtherActor)->HitSideEvent(endPoint);
					UGameplayStatics::ApplyDamage(Cast<ANPC_Cpp>(OtherActor), (float)(AttackCount + 1 * 30.f), nullptr, this, NULL);

					if (UKismetMathLibrary::RandomBoolWithWeight(.1f)) {
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), s_Wukong_Hit, endPoint);
					}
				}
		}
	}
}

// Called to bind functionality to input
void AWukongCharracter_Cpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWukongCharracter_Cpp::Jump_Function);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AWukongCharracter_Cpp::StopJumping_Function);

	PlayerInputComponent->BindAction("DistractNoise", IE_Pressed, this, &AWukongCharracter_Cpp::NoiseDistraction);

	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &AWukongCharracter_Cpp::UltimateAction);

	PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &AWukongCharracter_Cpp::EvadeAction);

	PlayerInputComponent->BindAction("PokeHit", IE_Pressed, this, &AWukongCharracter_Cpp::PokeHit);

	PlayerInputComponent->BindAction("EvadeAction", IE_Pressed, this, &AWukongCharracter_Cpp::EvadeAction);

	PlayerInputComponent->BindAction("Q_Slam", IE_Pressed, this, &AWukongCharracter_Cpp::EvadeAction);

	PlayerInputComponent->BindAction("SimpleHit", IE_Pressed, this, &AWukongCharracter_Cpp::SimpleHit);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWukongCharracter_Cpp::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWukongCharracter_Cpp::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWukongCharracter_Cpp::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWukongCharracter_Cpp::LookUpAtRate);



	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWukongCharracter_Cpp::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWukongCharracter_Cpp::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWukongCharracter_Cpp::OnResetVR);
}

void AWukongCharracter_Cpp::StopJumping_Function() {
	StopJumping();
}

/*HANDLES ULTIMATE CALLING AND CASTING*/
void AWukongCharracter_Cpp::UltimateAction()
{
	if (UltiRdy) {
		UltiCallingAndCasting = Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->UltiCall = true;
		UltiRdy = false;
		UltiCooldown = ComputeCooldown(7.f);
	}
}

/*HANDLES CLONES SPAWNING*/
void AWukongCharracter_Cpp::UltiCast()
{
	UltiActive = true;
	UltiTimer = ComputeCooldown(10.f);
	USphereComponent* temp = spheresArray.operator[](2);

	for (USphereComponent* eachSphere : spheresList) {//GETS EACH OF THE SPHERES

		FTransform SpawnTransform(UKismetMathLibrary::Conv_VectorToRotator(eachSphere->GetRightVector()), eachSphere->GetComponentLocation(), WukongMesh->GetComponentScale());
		FActorSpawnParameters tempSpawnP;
		tempSpawnP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AWukongClone_Cpp* cloneSpawn = GetWorld()->SpawnActor<AWukongClone_Cpp>(AWukongClone_Cpp::StaticClass(),SpawnTransform,tempSpawnP);

		cloneSpawn->setSpawnLocation(eachSphere->GetComponentLocation());
		cloneSpawn->Tags = { TEXT("WukongClone_Cpp") };
	}
}

void AWukongCharracter_Cpp::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWukongCharracter_Cpp::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

/*MOVE CHARRACTER IF NOT STUNNED OR NOT CASTINGG ULTI*/
void AWukongCharracter_Cpp::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (!Stunned&&!UltiActive&&!UltiCallingAndCasting&&!(Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->KnockedBack)) {
			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

/*MOVE CHARRACTER IF NOT STUNNED OR NOT CASTINGG ULTI*/
void AWukongCharracter_Cpp::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (!Stunned&&!UltiActive&&!UltiCallingAndCasting&&!(Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->KnockedBack)) {
			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

/*EVADE ACTION*/
void AWukongCharracter_Cpp::EvadeAction() {
	FlipFlopEvade = !FlipFlopEvade;
	if (FlipFlopEvade) {
		if (GetCharacterMovement()->IsMovingOnGround()) {
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			isEvading = true;
		}
	}
	else if (!FlipFlopEvade) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		isEvading = false;
	}
}

//DAMAGE RECEIVED
void AWukongCharracter_Cpp::DamageReceivedEvent(float Damage_p)
{
	if (UltiCallingAndCasting || UltiActive) {
		HPRemaining -= (int)(Damage_p / 2);
	}
	else
	{
		HPRemaining -= (int)Damage_p;
	}
}

/*IF GOT KILLED ENEMY TAUNTS AND QUIT GAME*/
void AWukongCharracter_Cpp::DestroyChar()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), p_Wukong_Death, this->GetActorLocation(), FRotator::ZeroRotator, FVector(5.f, 5.f, 5.f));
	UKismetSystemLibrary::Delay(GetWorld(), 5.f, FLatentActionInfo::FLatentActionInfo());
	this->K2_DestroyActor();
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(this->GetController()), EQuitPreference::Quit, true);
}

void AWukongCharracter_Cpp::EmoteAction()
{
	FlipFlopEmote = !FlipFlopEmote;
	if (!FlipFlopEmote) {
		Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->Emote = true;
	}
	else{
		Cast<UWukongAnimEventGraph_Cpp>(WukongMesh->GetAnimInstance())->Emote = false;
	}
}

float AWukongCharracter_Cpp::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageReceivedEvent(DamageAmount);
	return DamageAmount;
}

void AWukongCharracter_Cpp::HitSideEvent(FVector HitVector_p)
{
	FVector HitVector = HitVector_p;
	FVector DirectionVector;
	FVector capsuleLocation = GetCapsuleComponent()->GetComponentLocation();
	FRotator capsuleRotation = GetCapsuleComponent()->GetComponentRotation();

	DirectionVector = capsuleLocation - (UKismetMathLibrary::GetRightVector(capsuleRotation) + capsuleLocation);
	float sqrtForNormalization = sqrtf(powf(HitVector.X, 2) + powf(HitVector.Y, 2) + powf(HitVector.Z, 2));

	/*NORMALIZED HITVECTOR*/
	HitVector = FVector(HitVector.X / sqrtForNormalization, HitVector.Y / sqrtForNormalization, HitVector.Z / sqrtForNormalization);

	/*NORMALIZED DIRECTIONVECTOR*/
	DirectionVector = FVector(DirectionVector.X / sqrtForNormalization, DirectionVector.Y / sqrtForNormalization, DirectionVector.Z / sqrtForNormalization);


	if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), -1, -.7, true, true)) {
		Front = 1.0;
		UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
		Front = 0;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), .7, 1, true, true)) {
		Back = 1;
		UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
		Back = 0;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), -.7, .7, true, true)) {

		if (UKismetMathLibrary::Cross_VectorVector(HitVector, DirectionVector).Z<0) {
			Right = 1;
			UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
			Right = 0;
		}
		else if (UKismetMathLibrary::Cross_VectorVector(HitVector, DirectionVector).Z>0) {
			Left = 1;
			UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
			Left = 0;
		}
	}
}

/*WHEN Q SLAM TRACE SPHERE CREATED*/
void AWukongCharracter_Cpp::OnQslamNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload & BranchingPointPayload)
{
	TArray<FHitResult> outHit_vslam;
	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), WukongMesh->GetSocketLocation(TEXT("FX_Staff_Tip_B")), WukongMesh->GetForwardVector()*50.f, 650.f, objects, false, IgnoreActors, EDrawDebugTrace::None,outHit_vslam,true);
	for (FHitResult eachComp : outHit_vslam) {
		if (Cast<USceneComponent>(eachComp.Component)->IsSimulatingPhysics(NAME_None)) {/*APPLIES IMPULSE ON DESTRUCTIBLE AND PHYSICS COMPONENTS*/

			if (Cast<UStaticMeshComponent>(eachComp.Component) != nullptr) {
				Cast<UStaticMeshComponent>(eachComp.Component)->AddImpulseAtLocation(eachComp.ImpactPoint*FVector(0, 0, UKismetMathLibrary::RandomFloatInRange(100.f, 180.f)), eachComp.Location, NAME_None);
			}

			if (Cast<UDestructibleComponent>(eachComp.Component) != nullptr) {
				Cast<UDestructibleComponent>(eachComp.Component)->AddImpulseAtLocation(eachComp.ImpactPoint*FVector(0, 0, UKismetMathLibrary::RandomFloatInRange(100.f, 180.f)), eachComp.Location, NAME_None);
			}
		}

		if (UKismetSystemLibrary::GetDisplayName(Cast<UObject>(eachComp.Actor)) == "NPC_Cpp") {/*STUNS NPC CHARRACTER*/
			if (Cast<ANPC_Cpp>(eachComp.Actor)->getTeleport()) {
				UGameplayStatics::ApplyDamage(Cast<ANPC_Cpp>(eachComp.Actor), 100.f, nullptr, this, NULL);
			}
			else {
				Cast<ANPC_Cpp>(eachComp.Actor)->GetCharacterMovement()->StopMovementImmediately();
				Cast<ANPC_Cpp>(eachComp.Actor)->KwangMesh->Stop();
				Cast<ANPC_Cpp>(eachComp.Actor)->StStart = true;
				Cast<ANPC_Cpp>(eachComp.Actor)->StunRecoveryTimer = ComputeCooldown(10.f);
				Cast<ANPC_Cpp>(eachComp.Actor)->GetCharacterMovement()->StopMovementImmediately();
				UGameplayStatics::ApplyDamage(Cast<ANPC_Cpp>(eachComp.Actor), 180.f, nullptr, this, NULL);
				Cast<ANPC_Cpp>(eachComp.Actor)->GetController()->DisableInput(nullptr);
			}
		}
	}
}

/*BEING CALLED WITH THE INPUT*/
void AWukongCharracter_Cpp::Q_SlamEvent()
{
	if (!GetMovementComponent()->IsFalling()&&!Qslam&&!isEvading&&!Stunned&&!UltiActive&&!UltiCallingAndCasting) {
		UAnimInstance* qslamAnim = WukongMesh->GetAnimInstance();
		qslamAnim->Montage_Play(hitSlam);
		Qslam = true;
		slamCooldown = ComputeCooldown(10.f);
		IsAttacking = false;
		/*WHEN NOTIFY BEGINS USES THIS DELEGATED FUNTION*/
		qslamAnim->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnQslamNotifyBegin);

	}
}