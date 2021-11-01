#pragma once

#include "Runtime/Core/Public/UObject/NameTypes.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"

namespace bb_keys
{
	TCHAR const* const target_location = TEXT("TargetLocation");
	TCHAR const* const target_poi = TEXT("TargetPOI");
	TCHAR const* const seen_player = TEXT("SeenPlayer");
	TCHAR const* const is_in_POI = TEXT("IsInPOI");
	TCHAR const* const is_investigating = TEXT("IsInvestigating");
	TCHAR const* const kill_player = TEXT("KillPlayer");
	TCHAR const* const search_lockers_unlocked = TEXT("SearchLockersUnlocked");
	TCHAR const* const if_locker_in_POI = TEXT("IfLockerInPOI");
	TCHAR const* const should_search_locker = TEXT("ShouldSearchLocker");
	TCHAR const* const is_start = TEXT("IsStart");
	TCHAR const* const knows_throwable = TEXT("KnowsThrowable");
	TCHAR const* const heard_locker = TEXT("HeardLocker");
	TCHAR const* const is_stalking = TEXT("IsStalking");
	TCHAR const* const locker_flashlight = TEXT("LockerFlashlight");
}