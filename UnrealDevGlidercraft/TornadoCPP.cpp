// Fill out your copyright notice in the Description page of Project Settings.


#include "TornadoCPP.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/GameEngine.h"
#include "Animation/AnimInstance.h"



#include "UObject/ConstructorHelpers.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"


#include "Tornado_MovementComponent.h"



/*
	FVector:
	X    Y     Z
	Roll Pitch Yaw
	--------------
	FRotator:
	Up    L/R Barrel
	Pitch Yaw Roll
	Y     Z   X
*/

ATornadoCPP::ATornadoCPP(){
	PrimaryActorTick.bCanEverTick = true; // Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it.

	Rooter = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TornadosModel"));
	RootComponent = Rooter;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = camera.targetArmLength;
	SpringArm->TargetOffset = camera.targetOffset;
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bEditableWhenInherited = true;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	CollidingMovementComponent = CreateDefaultSubobject<UTornado_MovementComponent>(TEXT("MovementCollision"));
	CollidingMovementComponent->SetUpdatedComponent(Rooter);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

UPawnMovementComponent* ATornadoCPP::GetMovementComponent() const {return CollidingMovementComponent;}

// Called when the game starts or when spawned
void ATornadoCPP::BeginPlay(){
	Super::BeginPlay();
	
	DirectionMovement  = GetActorRotation();
	AdjustingDirection = GetActorRotation();
}

// Called to bind functionality to input
void ATornadoCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Tornado_MoveX"), this, &ATornadoCPP::Input_ForwardMovement);
	PlayerInputComponent->BindAxis(TEXT("Tornado_MoveY"), this, &ATornadoCPP::Input_SidewaysMovement);
	PlayerInputComponent->BindAxis(TEXT("Tornado_MoveZ"), this, &ATornadoCPP::Input_UpDownMovement);
	PlayerInputComponent->BindAxis(TEXT("Tornado_SpeedUp"), this, &ATornadoCPP::Input_SpeedUp);

	PlayerInputComponent->BindAxis(TEXT("Tornado_CameraX"), this, &ATornadoCPP::Input_CameraXMovement);
	PlayerInputComponent->BindAxis(TEXT("Tornado_CameraY"), this, &ATornadoCPP::Input_CameraYMovement);

	PlayerInputComponent->BindAction(TEXT("QuickStepL"), IE_Pressed, this, &ATornadoCPP::Input_DodgeL);
	PlayerInputComponent->BindAction(TEXT("QuickStepR"), IE_Pressed, this, &ATornadoCPP::Input_DodgeR);
}

FVector ATornadoCPP::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (!bDead){
		bInterpCamera = true;
		InterpCameraSpeed = InterpCameraSpeedInit;

		PushDir = Hit.Normal;
		FVector adjDir = AdjustingDirection.Vector();
		adjDir.Normalize(1.f);
	
		FVector tempPushDir = PushDir;

		float product  = 1.f - FMath::Abs(tempPushDir | adjDir);
		float sproduct = 1.f - FMath::Abs(tempPushDir | GetActorRightVector());
		float uproduct = 1.f - FMath::Abs(tempPushDir | GetActorUpVector());
		float fproduct = 1.f - FMath::Abs(tempPushDir | OutsideForce.GetSafeNormal(0.1f));
		float dproduct = 1.f - FMath::Abs(tempPushDir | GetActorRightVector());
		pushSpeed = (currentForwardSpeed+currentSidewaysSpeed+currentUpDownSpeed+OutsideForce.Size()+dodgeCurSpeed)*0.2f;

		PushDir.Normalize(1.f);
		float DmgAmmount = FMath::Abs(pushSpeed * 0.006) * FMath::Clamp(FMath::Abs((adjDir | PushDir))/6.f, 0.05f, 1.f);
		
		if (DmgAmmount > 0.5f) {
			Damage(FMath::RoundHalfToZero(DmgAmmount), true);
		}

		FVector dirReflect = FMath::GetReflectionVector(adjDir, Hit.Normal); 
		
		DirectionMovement = dirReflect.Rotation();

		currentForwardSpeed		*= product;
		currentSidewaysSpeed	*= sproduct;
		currentUpDownSpeed		*= uproduct;
		dodgeCurSpeed			*= -dproduct/2.f;
		OutsideForce			= (OutsideForce.GetSafeNormal(0.1f)) * fproduct;
		
		return dirReflect*pushSpeed;
	}
	
 //If dropped dead
	BlownOnHit(true);
	return FVector{0.f};
}

void ATornadoCPP::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//D(TEXT("Bruh Overlap"));
}

/*
* MAIN FUNCTIONS
*/

// Called every frame
void ATornadoCPP::Tick(float DT) {
	Super::Tick(DT);
	timer += DT;

	UpdateMovement(DT);
	UpdateComponents(DT);
	UpdateCamera(DT);
	DodgeCheck(DT);
	DeathFall(DT);

	if (Rooter->IsSimulatingPhysics()) {
		Rooter->AddImpulse(FVector{0.f, 0.f, -500.f}, NAME_None, true);
	}
	if (HP_HitDelay != 0.f){ 
		HP_HitDelay = FMath::Max(HP_HitDelay - DT, 0.f);
	}
	if (HP_SurfaceHitDelay != 0.f) {
		HP_SurfaceHitDelay = FMath::Max(HP_SurfaceHitDelay - DT, 0.f);
	}

	//Interps spring arm back to center
	SpringArm->SetRelativeLocation(FMath::VInterpTo(SpringArm->GetRelativeLocation(), FVector(0.f, 0.f, 0.f), DT, 2.f));
}

/** Updating the position of Actor's components*/
void ATornadoCPP::UpdateComponents(float DT){
	//Adjusting group offset to the turn speed for the dynamic purpose
	float pitchBasedFading = FMath::Pow(1-FMath::Abs(AdjustingDirection.Pitch)/70.f, 0.25);

	float turnSRollFactor  = FMath::Pow(FMath::Abs(currentSidewaysSpeed)/maxSidewaysSpeed, 0.8f) * 20.f*FMath::Sign(currentSidewaysSpeed);
	
	FRotator turnFactor{
		FMath::Pow(FMath::Abs(CurrentTrunSpeed.Pitch/turnSpeedClamp), 0.5f) * 40.f*FMath::Sign(CurrentTrunSpeed.Pitch)*pitchBasedFading,
		FMath::Pow(FMath::Abs(CurrentTrunSpeed.Yaw/turnSpeedClamp),	  1.2f) * 70.f*FMath::Sign(CurrentTrunSpeed.Yaw),
		FMath::Pow(FMath::Abs(CurrentTrunSpeed.Yaw/turnSpeedClamp),	  0.8f) * 50.f*FMath::Sign(CurrentTrunSpeed.Yaw)
	};

	OffsetGroupAdjustment.Pitch	= FMath::FInterpTo(OffsetGroupAdjustment.Pitch, turnFactor.Pitch,				 DT, 3.f);
	OffsetGroupAdjustment.Yaw	= FMath::FInterpTo(OffsetGroupAdjustment.Yaw,   turnFactor.Yaw,					 DT, 3.f);
	OffsetGroupAdjustment.Roll	= FMath::FInterpTo(OffsetGroupAdjustment.Roll,  turnFactor.Roll*1.5f+turnSRollFactor, DT, 3.f);

	/* CAMERA */

	//Fake Camera Lag (fixes motion blur glitches)
	camera.offset.Y = FMath::FInterpTo(camera.offset.Y, (CurrentTrunSpeed.Yaw/turnSpeedClamp)   * camera.lagSpeed.Y, DT, 2.f);					
	camera.offset.Z = FMath::FInterpTo(camera.offset.Z, (CurrentTrunSpeed.Pitch/turnSpeedClamp) * camera.lagSpeed.Z*10.f, DT, 2.f);
	
	//Camera distance deppending on speed
	camera.speedFactor = FMath::FInterpTo(camera.speedFactor, ((FMath::Clamp(currentForwardSpeed, 0.f, maxForwardSpeed))/maxForwardSpeed)*300.f, DT, 2.f);
	//ZoomIn on turn
	camera.distanceFactor = FMath::FInterpTo(camera.distanceFactor, 1.f-FMath::Abs((CurrentTrunSpeed.Yaw/turnSpeedClamp)*(FMath::Max(currentForwardSpeed/maxForwardSpeed, 0.f)))*0.5, DT, 2.f);
	
	
	//Camera wiggle
	float wiggleFactor = FMath::Pow(FMath::Abs(currentForwardSpeed/maxForwardSpeed/2.f)*2.f, 1.4f);

	wiggle.pos.X = FMath::FInterpTo(wiggle.pos.X, FMath::PerlinNoise1D(12347.f	+timer*wiggle.speed)*wiggle.amplitude*wiggleFactor, DT, wiggle.smoothing);
	wiggle.pos.Y = FMath::FInterpTo(wiggle.pos.Y, FMath::PerlinNoise1D(7568342.f+timer*wiggle.speed)*wiggle.amplitude*wiggleFactor, DT, wiggle.smoothing);
	wiggle.pos.Z = FMath::FInterpTo(wiggle.pos.Z, FMath::PerlinNoise1D(-93409.f	+timer*wiggle.speed)*wiggle.amplitude*wiggleFactor, DT, wiggle.smoothing);
	
	SpringArm->TargetArmLength = (camera.targetArmLength+camera.speedFactor)*camera.distanceFactor;
	SpringArm->TargetOffset = FVector(
		wiggle.pos.X+camera.targetOffset.X,
		wiggle.pos.Y+camera.targetOffset.Y-camera.offset.Y, 
		wiggle.pos.Z+camera.targetOffset.Z+camera.offset.Z*pitchBasedFading
	);

	//FOV
	cameraFOV = FMath::FInterpTo(cameraFOV, camera.initFOV+camera.FOVAdd*(FMath::Max(currentForwardSpeed, 0.f)/maxForwardSpeed), DT, 3.0f);
}

void ATornadoCPP::UpdateMovement(float DT) {
	RoughDirectionMovement(DT);
	
	//Not simulating physics nor dead
	pushSpeed = FMath::FInterpTo(pushSpeed, 0.0f, DT, pushDecMult);
	AdjustingDirection.Pitch += AngleDiff(DirectionMovement.Pitch, AdjustingDirection.Pitch) * turnAdjSpeed * DT;
	AdjustingDirection.Yaw   += AngleDiff(DirectionMovement.Yaw, AdjustingDirection.Yaw) * turnAdjSpeed * DT;
	AdjustingDirection.Roll  += AngleDiff(DirectionMovement.Roll, AdjustingDirection.Roll) * turnAdjSpeed * DT;
	
	//Smoothing rotation
	if (!FMath::IsNearlyZero(pushSpeed)) {
		DirectionMovement.Pitch += AngleDiff(AdjustingDirection.Pitch, DirectionMovement.Pitch) * 4 * DT;
		DirectionMovement.Yaw += AngleDiff(AdjustingDirection.Yaw, DirectionMovement.Yaw) * 4 * DT;
	}
	
	//For Rotations
	SetActorRotation(DirectionMovement);

	//Interp OutsideForce
	OutsideForce = FMath::VInterpTo(OutsideForce, OutsideForceTarget, DT, 1.5f);

	//For Hit events
	AddActorLocalOffset((FVector(0.f, 0.f, 0.f)) * DT, false);

	dodgeCurSpeed = FMath::FInterpTo(dodgeCurSpeed, 0.f, DT, 3.f);
	
	//For Movement
	CollidingMovementComponent->AddInputVector(GetActorForwardVector() * currentForwardSpeed);
	CollidingMovementComponent->AddInputVector(PushDir * pushSpeed);
	CollidingMovementComponent->AddInputVector(GetActorRightVector() * currentSidewaysSpeed);
	CollidingMovementComponent->AddInputVector(GetActorUpVector() * currentUpDownSpeed);
	CollidingMovementComponent->AddInputVector(GetActorRightVector() * dodgeCurSpeed);
	CollidingMovementComponent->AddInputVector(OutsideForce);
}


void ATornadoCPP::RoughDirectionMovement(float DT) {
	//Applying rough movement 
	if (!(Rooter->IsSimulatingPhysics())) {
		DirectionMovement += CurrentTrunSpeed * DT;
		//Stabilization	to the horizon
		DirectionMovement.Roll  = FMath::FInterpTo(DirectionMovement.Roll, 0.0f, DT, 1.8f);
		//Clamp the upward ant downward directions
		DirectionMovement.Pitch = FMath::Clamp(DirectionMovement.Pitch, -70.0f, 70.0f);
	}
}

void ATornadoCPP::Damage(int minushp, bool surface_hit) {
	if ((HP - minushp) <= 0) {
		HP = 0;
		bDead = true;
		return;
	}

	if (HP_HitDelay != 0.f && !surface_hit) { return; }
	if (HP_SurfaceHitDelay != 0.f && surface_hit) { return; }

	HP = HP - minushp;

	if (currentForwardSpeed > minForwardSpeed) {
		PlayMontage(BoostDamageMontage);
	} else {
		PlayMontage(DamageMontage);
	}

	if (surface_hit) {
		HP_SurfaceHitDelay = HP_HitDelayMax;
	} else {
		HP_HitDelay = HP_HitDelayMax;
	}
}

void ATornadoCPP::UpdateCamera(float DT) {
	if (bInterpCamera && !GetControlRotation().Equals(GetActorRotation(), 0.5f)) {
		GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), GetActorRotation(), DT, InterpCameraSpeed));
		InterpCameraSpeed += 0.2f;
	} else if (!bDead) {      
		GetController()->SetControlRotation(GetActorRotation());
		bInterpCamera = false;
	}
}

void ATornadoCPP::DeathFall(float DT) {
	if (bDead){
		RoughDirectionMovement(DT);

		if (bFallDoOnce) {
			Rooter->SetPhysicsAsset(DestroyedPhysicsAsset);
			//D(TEXT("dead"), 0.1f);
			bInputDisable = true;
			bFallDoOnce = false;     //resets OnRespawn()
			OnStartFall();
		}

		bInterpCamera = true;
		InterpCameraSpeed = InterpCameraSpeedInit;

		GravityForce = FVector{0.f, 0.f, FMath::FInterpTo(GravityForce.Z, -9000.f, DT, 1.f)};
		
		//Apply Gravity
		CollidingMovementComponent->AddInputVector(GravityForce);

		float FinalAngle = 69.f;
		if (currentForwardSpeed > 0.f) { FinalAngle *= -1; };

		if (DirectionMovement.Pitch > 0) {
			DirectionMovement.Pitch = FMath::FInterpTo(DirectionMovement.Pitch, FinalAngle, DT, 0.5f);
		}

		DirectionMovement = FMath::RInterpTo(DirectionMovement, FRotator{FinalAngle, DirectionMovement.Yaw, DirectionMovement.Roll}, DT, 1.f);
	}
}
	 
void ATornadoCPP::BlownOnHit(bool SimPhysics) {
	if (bDead && bBlownDoOnce) {
		Rooter->SetSimulatePhysics(SimPhysics);
		GravityForce = FVector{ 0.f };
		currentForwardSpeed = 0.f;
		bBlownDoOnce = false;       //resets OnRespawn()
		OnBlownUp();
	}
}

void ATornadoCPP::ResetsOnRespawn() {
	HP = maxHP;
	bDead = false;
	bFallDoOnce = true;
	bBlownDoOnce = true;
	bInputDisable = false;
}

void ATornadoCPP::BlowUpImmediately() {
		
	bFallDoOnce = false;				//resets on respawn

	Damage(HP, false);
	bDead = true;
	bInputDisable = true;
	BlownOnHit(false);
}


void ATornadoCPP::DodgeCheck(float DT) {
	if (isDodged && FMath::Abs(dodgeCurSpeed) <= dodgePushSpeed / 10.f) {
		isDodged = false;
	}else if(isDodged){
		FVector CameraCurrentOffset = SpringArm->GetRelativeLocation();
		SpringArm->SetRelativeLocation(FMath::VInterpTo(CameraCurrentOffset, FVector(0.f, -dodgeCurSpeed/10.f, 0.f), DT, 1.f));
	}
}

void ATornadoCPP::PlayMontage(UAnimMontage* montage) {
	if (montage != nullptr && Rooter->GetAnimInstance() != nullptr) {
		Rooter->GetAnimInstance()->Montage_Play(montage, 1.f);
	};
}

/*
* CONTROLS SETUP
*/ 


void ATornadoCPP::Input_DodgeL() {
	if (isDodged || bInputDisable) {return;}
	if (!bCtrlsOff){return;}

	Input_DodgeLogic(-1);
	PlayMontage(DodgeLMontage);
}

void ATornadoCPP::Input_DodgeR() {
	if (isDodged || bInputDisable) {return;}
	if (!bCtrlsOff){return;}

	Input_DodgeLogic(1);
	PlayMontage(DodgeRMontage);
}

void ATornadoCPP::Input_DodgeLogic(float input) {
	input_dodge = input;
	if (!bCtrlsOff){return;}

	if (input != 0.f && !isDodged){
		float DT = GetWorld()->GetDeltaSeconds();
		dodgeCurSpeed = dodgePushSpeed*input;
		isDodged = true;
	}
}

void ATornadoCPP::Input_ForwardMovement(float input) {
	input_moveX = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (input != 0 && !bInputDisable) {
		currentForwardSpeed = FMath::Clamp(currentForwardSpeed + input * forwardSpeedAcc * DT, -minForwardSpeed/2, currentMaxSpeed);
	} else if (input_speedUp == 0){
		currentForwardSpeed = FMath::FInterpTo(currentForwardSpeed, 0.0f, DT, slowDownMultiplyer);
	}
}

void ATornadoCPP::Input_SpeedUp(float input) {
	input_speedUp = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (input != 0 && !bInputDisable) {
		currentMaxSpeed = maxForwardSpeed;
		currentForwardSpeed = FMath::Min(currentForwardSpeed + forwardSpeedAcc * DT, currentMaxSpeed);
	} else {
		currentMaxSpeed = FMath::Max(currentMaxSpeed - forwardSpeedAcc * DT, minForwardSpeed);
	}

	//Getanims.speedUp
}

void ATornadoCPP::Input_SidewaysMovement(float input) {
	input_moveY = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (input != 0 && input_speedUp == 0 && !bInputDisable) {
		currentSidewaysSpeed = FMath::Clamp(currentSidewaysSpeed + input * sidewaysSpeedAcc * DT, -maxSidewaysSpeed, maxSidewaysSpeed);
	}
	else if (!bInputDisable) {
		currentSidewaysSpeed = FMath::FInterpTo(currentSidewaysSpeed, 0.0f, DT, 1.0f);
	}
}

void ATornadoCPP::Input_UpDownMovement(float input) {
	input_moveY = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (input != 0 && input_speedUp == 0 && !bInputDisable) {
		currentUpDownSpeed = FMath::Clamp(currentUpDownSpeed + input * sidewaysSpeedAcc * DT, -maxSidewaysSpeed, maxSidewaysSpeed);
	}
	else if (!bInputDisable) {
		currentUpDownSpeed = FMath::FInterpTo(currentUpDownSpeed, 0.0f, DT, 1.0f);
	}
}

//Turn L/R - Roll
void ATornadoCPP::Input_CameraXMovement(float input) {
	input_cameraX = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (!bInputDisable){
		if (input != 0) {
			float YawTarget = FMath::Clamp(input * turnSpeed, -turnSpeedClamp, turnSpeedClamp);
			CurrentTrunSpeed.Yaw = FMath::FInterpTo(CurrentTrunSpeed.Yaw, YawTarget, DT, 1.0f);

			float RollTarget = FMath::Clamp(CurrentTrunSpeed.Yaw*2.0f, -turnSpeedClamp, turnSpeedClamp);
			CurrentTrunSpeed.Roll = FMath::FInterpTo(CurrentTrunSpeed.Roll, (RollTarget/turnSpeed)*70.0f, DT, 1.0f);
		} else {
			CurrentTrunSpeed.Yaw  = FMath::FInterpTo(CurrentTrunSpeed.Yaw, 0.0f, DT, turnDecSpeed);
			CurrentTrunSpeed.Roll = FMath::FInterpTo(CurrentTrunSpeed.Roll, 0.0f, DT, turnDecSpeed);
		}
	}
}

//Turn Up/Down - Pitch
void ATornadoCPP::Input_CameraYMovement(float input) {
	input_cameraY = input;
	if (!bCtrlsOff){return;}

	float DT = GetWorld()->GetDeltaSeconds();
	if (!bInputDisable) {
		if (input != 0) {
			float PitchTarget = FMath::Clamp(input * -turnSpeed / 2.f + FMath::Abs(CurrentTrunSpeed.Yaw) * -0.2f, -turnSpeedClamp, turnSpeedClamp);
			CurrentTrunSpeed.Pitch = FMath::FInterpTo(CurrentTrunSpeed.Pitch, PitchTarget, DT, 1.0f);
		}
		else {
			CurrentTrunSpeed.Pitch = FMath::FInterpTo(CurrentTrunSpeed.Pitch, 0.0f, DT, turnDecSpeed);
		}
	}
}

