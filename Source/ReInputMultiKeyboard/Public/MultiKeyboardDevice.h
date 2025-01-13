#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"
#include "Windows/WindowsApplication.h"
#include "ReInputTypes.h"

REINPUTMULTIKEYBOARD_API DECLARE_LOG_CATEGORY_EXTERN(LogReInputMultiKeyboard, Log, All);

class REINPUTMULTIKEYBOARD_API FMultiKeyboardDevice : public IInputDevice, IWindowsMessageHandler
{
	friend struct FMultiKeyboardDeviceData;
	
public:
	FMultiKeyboardDevice(TSharedRef<FGenericApplicationMessageHandler> InMessageHandler);
	~FMultiKeyboardDevice();

	bool IsGameViewportFocused();

	// IInputDevice
	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;

	// IWindowsMessageHandler
	virtual bool ProcessMessage(HWND Hwnd, uint32 Msg, WPARAM wParam, LPARAM lParam, int32& OutResult) override;

	// Multikeyboard handling
	HWND GetGameWindowHandle();
	static TArray<FString> GetCurrentDeviceIDs();
	static bool GetDeviceIDFromRawInput(HANDLE DeviceHandle, FString& DeviceName);
	static bool GetDeviceFriendlyNameFromRawInput(HANDLE RawInputHandle, FString& Manufacturer, FString& Product);
	
	void OnDeviceChangeEvent(WPARAM wParam, LPARAM lParam);
	void OnDeviceInputEvent(FString DeviceID, bool IsDown, PRAWINPUT RawInputData);

	void AddDevice(FString ID, FString FriendlyName);
	void RemoveDevice(FString ID);

	TMap<FString, FMultiKeyboardDeviceData> GetDevices() { return Devices; }
	FMultiKeyboardDeviceData* GetDeviceWithControllerID(int32 ControllerId) { return Devices.Find(DevicesIDs[ControllerId]); }
	FMultiKeyboardDeviceData* GetDeviceWithControllerID(FInputDeviceId ControllerId) { return Devices.Find(DevicesIDs[ControllerId.GetId()]); }

private:
	void AddDeviceIDMapping(int32 InternalID, FString ID);
	
	TMap<FString, FMultiKeyboardDeviceData> Devices;
	TMap<int32, FString> DevicesIDs;
	
	HWND HWnd = nullptr;
	UINT DwSize;
	RAWINPUTDEVICE Rid;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	int TotalDeviceCount = 0;
};
