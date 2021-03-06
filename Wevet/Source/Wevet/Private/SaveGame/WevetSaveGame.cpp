// Copyright © 2018 wevet works All Rights Reserved.


#include "SaveGame/WevetSaveGame.h"

UWevetSaveGame::UWevetSaveGame()
{
	// Set to current version, this will get overwritten during serialization when loading
	SavedDataVersion = ESaveGameVersion::LatestVersion;
}

void UWevetSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != ESaveGameVersion::LatestVersion)
	{
		if (SavedDataVersion < ESaveGameVersion::AddedItemData)
		{
			//
		}

		SavedDataVersion = ESaveGameVersion::LatestVersion;
	}
}

