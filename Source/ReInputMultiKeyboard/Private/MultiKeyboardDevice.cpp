#if UE_EDITOR
#pragma optimize("", off)
#endif

#include "MultiKeyboardDevice.h"
#include <SetupAPI.h>
#include "hidsdi.h"
#include "ReInputTypes.h"
#include "Widgets/SViewport.h"

DEFINE_LOG_CATEGORY(LogReInputMultiKeyboard);

FMultiKeyboardDevice::FMultiKeyboardDevice(TSharedRef<FGenericApplicationMessageHandler> InMessageHandler)
: MessageHandler( InMessageHandler )
{
	//Register this module to receive messages from the OS
	FWindowsApplication* WindowsApp = static_cast<FWindowsApplication*>( FSlateApplication::Get().GetPlatformApplication().Get() );
	check( WindowsApp );
	WindowsApp->AddMessageHandler( *this );

	//Setup Raw Input Device
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.dwFlags = RIDEV_DEVNOTIFY;
	Rid.hwndTarget = GetGameWindowHandle();

	RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

FMultiKeyboardDevice::~FMultiKeyboardDevice()
{
}

bool FMultiKeyboardDevice::IsGameViewportFocused()
{
	TSharedPtr<SViewport> ViewportWidget = FSlateApplication::Get().GetGameViewport();
	if (ViewportWidget.IsValid())
	{
		return ViewportWidget->HasFocusedDescendants();
	}
	
	return false;
}

void FMultiKeyboardDevice::Tick(float DeltaTime)
{
}

void FMultiKeyboardDevice::SendControllerEvents()
{
}

void FMultiKeyboardDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FMultiKeyboardDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FMultiKeyboardDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
}

void FMultiKeyboardDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{
}

bool FMultiKeyboardDevice::ProcessMessage(HWND Hwnd, uint32 Msg, WPARAM wParam, LPARAM lParam, int32& OutResult)
{
	switch (Msg)
	{
	case WM_INPUT:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			//UE_LOG(LogReInputMultiKeyboard, VeryVerbose, TEXT("WM_INPUT"));

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &DwSize, sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}
			
			LPBYTE Lpb = new BYTE[DwSize];
			if (Lpb == NULL)
			{
				break;
			}

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, Lpb, &DwSize, sizeof(RAWINPUTHEADER)) != DwSize)
			{
				delete[] Lpb;
				break;
			}
			
			PRAWINPUT RawInputData = PRAWINPUT(Lpb);
			if (RawInputData->header.dwType != RIM_TYPEKEYBOARD)
			{
				delete[] Lpb;
				break;
			}

			UINT Event = RawInputData->data.keyboard.Message;
			if (Event == WM_KEYDOWN || Event == WM_KEYUP)
			{
				HANDLE DeviceHandle = RawInputData->header.hDevice;
				FString DeviceID;
				if (GetDeviceIDFromRawInput(DeviceHandle, DeviceID))
				{
					OnDeviceInputEvent(DeviceID, Event == WM_KEYDOWN, RawInputData);
				}
			}
		}
		break;
	case WM_INPUT_DEVICE_CHANGE:
		{
			//UE_LOG(LogReInputMultiKeyboard, VeryVerbose, TEXT("WM_INPUT_DEVICE_CHANGE"));
			OnDeviceChangeEvent(wParam, lParam);
		}
		break;
	default:
		break;
	}
	
	return false;
}

HWND FMultiKeyboardDevice::GetGameWindowHandle()
{
	if (FSlateApplication::IsInitialized())
	{
		TSharedPtr<SWindow> MainWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (MainWindow.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWindow = MainWindow->GetNativeWindow();
			if (NativeWindow.IsValid())
			{
				return static_cast<HWND>(NativeWindow->GetOSWindowHandle());
			}
		}
	}

	return nullptr;
}

//This function sounds like it would just return the IDs of devices in the Devices map,
//but it actually returns the IDs of the devices that are currently connected.
TArray<FString> FMultiKeyboardDevice::GetCurrentDeviceIDs()
{
	UINT NumDevices;
	TArray<FString> ConnectedKeyboards;

	if (GetRawInputDeviceList(nullptr, &NumDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
			{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("GetRawInputDeviceList failed."));
		return ConnectedKeyboards;
	}
	
	RAWINPUTDEVICELIST* DeviceList = new RAWINPUTDEVICELIST[NumDevices];
	if (!DeviceList)
	{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Failed to allocate memory for device list."));
		return ConnectedKeyboards;
	}

	if (GetRawInputDeviceList(DeviceList, &NumDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
	{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("GetRawInputDeviceList failed."));
		delete[] DeviceList;
		return ConnectedKeyboards;
	}

	for (UINT I = 0; I < NumDevices; I++)
			{
		if (DeviceList[I].dwType == RIM_TYPEKEYBOARD)
		{
			UINT NameSize = 0;
			if (GetRawInputDeviceInfo(DeviceList[I].hDevice, RIDI_DEVICENAME, nullptr, &NameSize) == -1)
			{
				UE_LOG(LogReInputMultiKeyboard, Error, TEXT("GetRawInputDeviceInfo failed."));
			}
			else 
			{
				wchar_t* DeviceName = new wchar_t[NameSize];
				if (GetRawInputDeviceInfo(DeviceList[I].hDevice, RIDI_DEVICENAME, DeviceName, &NameSize) != -1)
				{
					ConnectedKeyboards.Add(DeviceName);
				}
				delete[] DeviceName;
			}

		}
	}
	
	delete[] DeviceList;

	return ConnectedKeyboards;
}

bool FMultiKeyboardDevice::GetDeviceIDFromRawInput(HANDLE DeviceHandle, FString& DeviceName)
{
	UINT Size;
	if (GetRawInputDeviceInfoA(DeviceHandle, RIDI_DEVICENAME, NULL, &Size) != 0)
	{
		return false;
	}

	char* RawDeviceName = new char[Size];
	if (GetRawInputDeviceInfoA(DeviceHandle, RIDI_DEVICENAME, RawDeviceName, &Size) <= 0)
	{
		delete[] RawDeviceName;
		return false;
	}
	
	HDEVINFO HDevInfo = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (HDevInfo == INVALID_HANDLE_VALUE)
	{
		delete[] RawDeviceName;
		return false;
	}
	
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	if (!SetupDiOpenDeviceInterfaceA(HDevInfo, RawDeviceName, 0, &DeviceInterfaceData))
	{
		delete[] RawDeviceName;
		SetupDiDestroyDeviceInfoList(HDevInfo);
		return false;
	}
	
	SP_DEVINFO_DATA DeviceInfoData =
			{};
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if (!SetupDiEnumDeviceInfo(HDevInfo, 0, &DeviceInfoData))
	{
		delete[] RawDeviceName;
		SetupDiDestroyDeviceInfoList(HDevInfo);
		return false;
	}

	char FriendlyName[256];
	DWORD SizeRequired;
	if (!SetupDiGetDeviceRegistryPropertyA(HDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME,
		NULL, (PBYTE)FriendlyName, sizeof(FriendlyName), &SizeRequired))
	{
		DeviceName = RawDeviceName;
	}
	else
	{
		DeviceName = FriendlyName;
	}

	delete[] RawDeviceName;
	SetupDiDestroyDeviceInfoList(HDevInfo);

	return true;
}

bool FMultiKeyboardDevice::GetDeviceFriendlyNameFromRawInput(HANDLE RawInputHandle, FString& Manufacturer, FString& Product)
{
	WCHAR DevicePath[512];
	UINT Size = sizeof(DevicePath);

	if (GetRawInputDeviceInfo(RawInputHandle, RIDI_DEVICENAME, DevicePath, &Size) < 0)
	{
		return false;
	}

	HANDLE DeviceHandle = CreateFile(DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		WCHAR Buffer[256] ={0};
		if (HidD_GetManufacturerString(DeviceHandle, Buffer, sizeof(Buffer)))
			{
			if (iswprint(Buffer[0]))
			{
				Manufacturer = Buffer;
			}
			else
			{
				Manufacturer = L"Unreadable Manufacturer";
			}
		}
		else
		{
			Manufacturer = L"Unreadable Manufacturer";
		}
		if (HidD_GetProductString(DeviceHandle, Buffer, sizeof(Buffer))) 
		{
			if (iswprint(Buffer[0]))
			{
				Product = Buffer;
			}
			else
			{
				Product = L"Unreadable Product";
			}
		}
		else
		{
			Product = L"Unreadable Product";
		}
		CloseHandle(DeviceHandle);
		return true;
	}

	return false;
}

void FMultiKeyboardDevice::OnDeviceChangeEvent(WPARAM wParam, LPARAM lParam)
{
	HANDLE DeviceHandle = reinterpret_cast<HANDLE>(lParam);
	FString DeviceName;
	FString DeviceFriendlyName = "Unknown Device";
	FString Manufacturer;
	FString Product;
	
	switch (wParam)
	{
	case GIDC_ARRIVAL:
		{
			if (GetDeviceIDFromRawInput(DeviceHandle, DeviceName))
			{
				UE_LOG(LogReInputMultiKeyboard, Log, TEXT("Added Device With ID: %s \n"), *DeviceName);

				if (GetDeviceFriendlyNameFromRawInput(DeviceHandle, Manufacturer, Product))
				{
					DeviceFriendlyName = Manufacturer + " - " + Product;
					UE_LOG(LogReInputMultiKeyboard, Log, TEXT("Found Device Name: %s \n"), *DeviceFriendlyName);
				}
				else 
				{
					UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Unable to find name for new device!"));
				}

				AddDevice(DeviceName, DeviceFriendlyName);
			}
			else
			{
				UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Failed to find ID for new device! Device will NOT be added and CANNOT be used!"));
			}
		}
		break;
	case GIDC_REMOVAL:
		{
			//A removal event doesn't contain any information about the removed device so we manually have to determine what is missing
			TArray<FString> ConnectedKeyboards = GetCurrentDeviceIDs();
			TArray<FString> LastTrackedKeyboards;
			Devices.GetKeys(LastTrackedKeyboards);

			for (FString DeviceID : LastTrackedKeyboards)
			{
				if (!ConnectedKeyboards.Contains(DeviceID))
				{
					RemoveDevice(DeviceID);
				}
			}
		}
		break;
	default:
		break;
	}
}

void FMultiKeyboardDevice::OnDeviceInputEvent(FString DeviceID, bool IsDown, PRAWINPUT RawInputData)
{
	FMultiKeyboardDeviceData* Device = Devices.Find(DeviceID);

	if (!Device)
	{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Received input event for device %s, but it was not found in the device list"), *DeviceID);
		return;
	}

	if (!Device->IsActive())
	{
		Device->Activate();
		AddDeviceIDMapping(Device->Index, Device->ID);
	}

	FString DeviceName = Device->FriendlyName.ToString();
	if (DeviceName.IsEmpty() || DeviceName.Contains(TEXT("Unknown")))
	{
		DeviceName = "KeyboardDevice";
	}

	//Set scope to correct keyboard
	FInputDeviceScope Scope(this, *DeviceName, Device->Index, "Keyboard");

	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	FPlatformUserId PlatformUser = PLATFORMUSERID_NONE;
	FInputDeviceId InputDevice = INPUTDEVICEID_NONE;

	DeviceMapper.RemapControllerIdToPlatformUserAndDevice(Device->Index, PlatformUser, InputDevice);

	//Setting flags so the keycodes are usable
	uint16_t ScanCode = RawInputData->data.keyboard.MakeCode;
	ScanCode |= (RawInputData->data.keyboard.Flags & RI_KEY_E0) ? 0xe000 : 0;
	ScanCode |= (RawInputData->data.keyboard.Flags & RI_KEY_E1) ? 0xe100 : 0;
	
	uint16_t VKCode = RawInputData->data.keyboard.VKey;

	//We can't just work with the VKey codes directly atm because Windows has some special cases
	switch (VKCode)
	{
	case VK_SHIFT:
	case VK_MENU:
	case VK_CONTROL:
		{
			VKCode = LOWORD(MapVirtualKeyW(ScanCode, MAPVK_VSC_TO_VK_EX));
		}
		break;
	default:
		break;
	}

	//Now we can work with the VKey
	if (FKey* ActionKey = VKeyToMKBKey.Find(VKCode))
	{
		if (IsDown)
		{
			//if (IsGameViewportFocused())
			{
				//If the key is already present consider it a held (repeat) key
				if (!Device->KeyStates.Contains(*ActionKey))
				{
					Device->KeyStates.Add(*ActionKey);
					MessageHandler->OnControllerButtonPressed(ActionKey->GetFName(), PlatformUser, InputDevice, false);
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("%s - Pressed - %s"), *DeviceName, *ActionKey->ToString()));
				}
				else
				{
					MessageHandler->OnControllerButtonPressed(ActionKey->GetFName(), PlatformUser, InputDevice, true);
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("%s - Held - %s"), *DeviceName, *ActionKey->ToString()));
				}
			}
		}
		else
		{
			if (Device->KeyStates.Contains(*ActionKey))
			{
				Device->KeyStates.Remove(*ActionKey);
				MessageHandler->OnControllerButtonReleased(ActionKey->GetFName(), PlatformUser, InputDevice, false);
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("%s - Released - %s"), *DeviceName, *ActionKey->ToString()));
			}
			//else
			//{
			//	UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Received key up input event for device %s, but the key %s was not found in the device's key state list"), *DeviceName, *ActionKey->ToString());
			//}
			
		}
	}
	else
	{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Received input event for device %s, but the VKey %d was not found in the VKey to MKBKey map"), *DeviceName, VKCode);
	}
}

void FMultiKeyboardDevice::AddDevice(FString ID, FString FriendlyName)
{
	if (Devices.Contains(ID))
	{
		UE_LOG(LogReInputMultiKeyboard, Error, TEXT("Tried to add device with ID %s, but it was already present"), *ID);
		return;
	}

	FMultiKeyboardDeviceData Device = FMultiKeyboardDeviceData(
		TotalDeviceCount,
		ID,
		FText::FromString(FriendlyName),
		EKeyboardDeviceState::CONNECTED);
	
	TotalDeviceCount++;
	Devices.Add(ID, Device);
}

void FMultiKeyboardDevice::RemoveDevice(FString ID)
{
	if (FMultiKeyboardDeviceData* Device = Devices.Find(ID))
	{
		if (Device->IsActive())
		{
			IPlatformInputDeviceMapper& deviceMapper = IPlatformInputDeviceMapper::Get();
			FPlatformUserId platformUser = PLATFORMUSERID_NONE;
			FInputDeviceId inputDevice = INPUTDEVICEID_NONE;

			deviceMapper.RemapControllerIdToPlatformUserAndDevice( Device->Index, platformUser, inputDevice );
			deviceMapper.Internal_MapInputDeviceToUser( inputDevice, platformUser, EInputDeviceConnectionState::Disconnected );

			DevicesIDs.Remove(Device->DeviceInternalID.GetId());
			Devices.Remove(ID);
		}
		else
		{
			//If the device isn't active there nothing to do beside cleanup our map
			Devices.Remove(ID);
		}
	}
}

void FMultiKeyboardDevice::AddDeviceIDMapping(int32 InternalID, FString ID)
{
	DevicesIDs.Add(InternalID, ID);
}
