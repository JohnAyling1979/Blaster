#include "CombatComponent.h"
#include "Blaster/Weapom/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.0f;
	AimingWalkSpeed = 450.0f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr)
	{
		return;
	}

	if (Controller == nullptr)
	{
		Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}

	if (Controller)
	{
		if (HUD == nullptr)
		{
			HUD = Controller->GetHUD<ABlasterHUD>();
		}
	}

	if (HUD)
	{
		FHUDPackage HUDPackage;

		if (EquippedWeapon == nullptr)
		{
			HUDPackage.CrosshairsCenter = nullptr;
			HUDPackage.CrosshairsLeft = nullptr;
			HUDPackage.CrosshairsRight = nullptr;
			HUDPackage.CrosshairsTop = nullptr;
			HUDPackage.CrosshairsBottom = nullptr;
		}
		else
		{
			HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
			HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
			HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
			HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
		}

		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;

		CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, 600.f),
			FVector2D(0.f, 1.f),
			Velocity.Size()
		);

		if (Character->GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirFactor = FMath::FInterpTo(
				CrosshairInAirFactor,
				2.25f,
				DeltaTime,
				2.25f
			);
		}
		else
		{
			CrosshairInAirFactor = FMath::FInterpTo(
				CrosshairInAirFactor,
				0.f,
				DeltaTime,
				30.f
			);
		}

		HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor;

		HUD->SetHUDPackage(HUDPackage);
	}

}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimingWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimingWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);

		ServerFire(HitResult.ImpactPoint);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr || Character == nullptr)
	{
		return;
	}

	Character->PlayFireMontage(bAiming);
	EquippedWeapon->Fire(TraceHitTarget);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2d CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldLocation, CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldLocation,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldLocation;
		FVector End = Start + (CrosshairWorldDirection * TRACE_LENGTH);

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}
