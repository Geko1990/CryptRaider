// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "Grabber.h"

#include "Engine/World.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle)
		return;

	UPrimitiveComponent* GrabbedComponent =  PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent) {
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle)
		return;

	UPrimitiveComponent* GrabbedComponent =  PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent) {
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle)
		return;

	FHitResult HitResult;
	bool hasHit = GetGrabbableInReach(HitResult);

	if (hasHit) {
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent, HitResult.BoneName, HitResult.ImpactPoint, GetComponentRotation());
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!Result) {
		UE_LOG(LogTemp, Error, TEXT("Grabber Requires a UPhysicsHandlerComponent!"));
	}
	return Result;
}

bool UGrabber::GetGrabbableInReach(FHitResult &HitResultOut) const
{
	UWorld* World = GetWorld();
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	DrawDebugLine(World, Start, End, FColor::Red);
	DrawDebugSphere(World, End, 10, 10, FColor::Blue, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return World->SweepSingleByChannel(
		HitResultOut, Start, End, FQuat::Identity, 
		ECC_GameTraceChannel2, Sphere);
}

