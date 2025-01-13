// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IInputDeviceModule.h"
#include "Modules/ModuleManager.h"

class FReInputMultiKeyboardModule : public IInputDeviceModule
{
public:
	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
