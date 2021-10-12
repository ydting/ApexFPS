// Copyright Epic Games, Inc. All Rights Reserved.

/* USE THIS IF DEVELOPING A PLUGINS*/
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FYDTPluginsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

/* USE THIS IF DEVELOPING A GAME MODULE*/

/*
 
#pragma once

#include "Modules/ModuleManager.h"

*/