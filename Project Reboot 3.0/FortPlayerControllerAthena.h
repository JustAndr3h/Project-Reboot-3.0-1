#pragma once

#include "FortPlayerController.h"
#include "FortPlayerStateAthena.h"
#include "FortPlayerPawn.h"
#include "SoftObjectPtr.h"
#include "FortKismetLibrary.h"
#include "AthenaMarkerComponent.h"
#include "FortVolume.h"

static void ApplyCID(AFortPlayerPawn* Pawn, UObject* CID, bool bUseServerChoosePart = false)
{
	if (!CID)
		return;

	if (!Pawn && bUseServerChoosePart)
		return;

	static auto HeroDefinitionOffset = CID->GetOffset("HeroDefinition");
	auto HeroDefinition = CID->Get(HeroDefinitionOffset);

	using UFortHeroSpecialization = UObject;

	static auto SpecializationsOffset = HeroDefinition->GetOffset("Specializations");
	auto& Specializations = HeroDefinition->Get<TArray<TSoftObjectPtr<UFortHeroSpecialization>>>(SpecializationsOffset);

	auto PlayerState = Pawn->GetPlayerState();

	for (int i = 0; i < Specializations.Num(); i++)
	{
		auto& SpecializationSoft = Specializations.at(i);

		static auto FortHeroSpecializationClass = FindObject<UClass>("/Script/FortniteGame.FortHeroSpecialization");
		auto Specialization = SpecializationSoft.Get(FortHeroSpecializationClass, true);

		if (Specialization)
		{
			static auto Specialization_CharacterPartsOffset = Specialization->GetOffset("CharacterParts");
			auto& CharacterParts = Specialization->Get<TArray<TSoftObjectPtr<UObject>>>(Specialization_CharacterPartsOffset);

			static auto CustomCharacterPartClass = FindObject<UClass>("/Script/FortniteGame.CustomCharacterPart");

			/* if (bUseServerChoosePart)
			{
				for (int z = 0; z < CharacterParts.Num(); z++)
				{
					Pawn->ServerChoosePart((EFortCustomPartType)z, CharacterParts.at(z).Get(CustomCharacterPartClass, true));
				}

				continue; // hm?
			} */

			bool aa;

			TArray<UObject*> CharacterPartsaa;

			for (int z = 0; z < CharacterParts.Num(); z++)
			{
				auto& CharacterPartSoft = CharacterParts.at(z);
				auto CharacterPart = CharacterPartSoft.Get(CustomCharacterPartClass, true);

				CharacterPartsaa.Add(CharacterPart);

				continue;
			}

			UFortKismetLibrary::ApplyCharacterCosmetics(GetWorld(), CharacterPartsaa, PlayerState, &aa);
			CharacterPartsaa.Free();
		}
	}

	static auto HeroTypeOffset = PlayerState->GetOffset("HeroType");
	// PlayerState->Get(HeroTypeOffset) = HeroDefinition;
}

struct FGhostModeRepData
{
	bool& IsInGhostMode()
	{
		static auto bInGhostModeOffset = FindOffsetStruct("/Script/FortniteGame.GhostModeRepData", "bInGhostMode");
		return *(bool*)(__int64(this) + bInGhostModeOffset);
	}

	UFortWorldItemDefinition*& GetGhostModeItemDef()
	{
		static auto GhostModeItemDefOffset = FindOffsetStruct("/Script/FortniteGame.GhostModeRepData", "GhostModeItemDef");
		return *(UFortWorldItemDefinition**)(__int64(this) + GhostModeItemDefOffset);
	}
};

class AFortPlayerControllerAthena : public AFortPlayerController
{
public:
	static inline void (*GetPlayerViewPointOriginal)(AFortPlayerControllerAthena* PlayerController, FVector& Location, FRotator& Rotation);
	static inline void (*ServerReadyToStartMatchOriginal)(AFortPlayerControllerAthena* PlayerController);
	static inline void (*ServerRequestSeatChangeOriginal)(AFortPlayerControllerAthena* PlayerController, int TargetSeatIndex);
	static inline void (*EnterAircraftOriginal)(UObject* PC, AActor* Aircraft);
	static inline void (*StartGhostModeOriginal)(UObject* Context, FFrame* Stack, void* Ret);
	static inline void (*EndGhostModeOriginal)(AFortPlayerControllerAthena* PlayerController);

	AFortPlayerStateAthena* GetPlayerStateAthena()
	{
		return (AFortPlayerStateAthena*)GetPlayerState();
	}

	FGhostModeRepData* GetGhostModeRepData()
	{
		static auto GhostModeRepDataOffset = GetOffset("GhostModeRepData");
		return GetPtr<FGhostModeRepData>(GhostModeRepDataOffset);
	}

	UAthenaMarkerComponent* GetMarkerComponent()
	{
		static auto MarkerComponentOffset = GetOffset("MarkerComponent");
		return Get<UAthenaMarkerComponent*>(MarkerComponentOffset);
	}

	static void StartGhostModeHook(UObject* Context, FFrame* Stack, void* Ret); // we could native hook this but eh
	static void EndGhostModeHook(AFortPlayerControllerAthena* PlayerController);
	static void EnterAircraftHook(UObject* PC, AActor* Aircraft);
	static void ServerRequestSeatChangeHook(AFortPlayerControllerAthena* PlayerController, int TargetSeatIndex); // actually in zone
	static void ServerRestartPlayerHook(AFortPlayerControllerAthena* Controller);
	static void ServerGiveCreativeItemHook(AFortPlayerControllerAthena* Controller, FFortItemEntry CreativeItem);
	static void ServerTeleportToPlaygroundLobbyIslandHook(AFortPlayerControllerAthena* Controller);
	static void ServerAcknowledgePossessionHook(APlayerController* Controller, APawn* Pawn);
	static void ServerPlaySquadQuickChatMessageHook(AFortPlayerControllerAthena* PlayerController, __int64 ChatEntry, __int64 SenderID);
	static void GetPlayerViewPointHook(AFortPlayerControllerAthena* PlayerController, FVector& Location, FRotator& Rotation);
	static void ServerReadyToStartMatchHook(AFortPlayerControllerAthena* PlayerController);

	static UClass* StaticClass()
	{
		static auto Class = FindObject<UClass>("/Script/FortniteGame.FortPlayerControllerAthena");
		return Class;
	}
};