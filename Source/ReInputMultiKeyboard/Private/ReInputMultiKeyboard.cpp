// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReInputMultiKeyboard.h"

#include "MultiKeyboardDevice.h"

#define LOCTEXT_NAMESPACE "FReInputMultiKeyboardModule"

TSharedPtr<class IInputDevice> FReInputMultiKeyboardModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	return MakeShareable(new FMultiKeyboardDevice(InMessageHandler));
}

void FReInputMultiKeyboardModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
}

void FReInputMultiKeyboardModule::ShutdownModule()
{
	IInputDeviceModule::ShutdownModule();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReInputMultiKeyboardModule, ReInputMultiKeyboard)