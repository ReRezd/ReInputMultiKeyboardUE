#include "ReInputTypes.h"

#include "MultiKeyboardDevice.h"

FMultiKeyboardDeviceData::FMultiKeyboardDeviceData()
{
	Index = -1;
	ID = "Unknown Device";
	FriendlyName = FText::FromString("Unknown Device Name");
	State = EKeyboardDeviceState::CONNECTED;
}

FMultiKeyboardDeviceData::FMultiKeyboardDeviceData(int InIndex, FString InID, FText InFriendlyName,
	EKeyboardDeviceState InState)
{
	Index = InIndex;
	ID = InID;
	FriendlyName = InFriendlyName;
	State = InState;
}

void FMultiKeyboardDeviceData::Activate()
{
	State = EKeyboardDeviceState::ACTIVATED;

	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	FPlatformUserId PlatformUser = PLATFORMUSERID_NONE;
	FInputDeviceId InputDevice = INPUTDEVICEID_NONE;
	
	DeviceMapper.RemapControllerIdToPlatformUserAndDevice( Index, PlatformUser, InputDevice );
	DeviceMapper.Internal_MapInputDeviceToUser( InputDevice, PlatformUser, EInputDeviceConnectionState::Connected );

	DeviceInternalID = InputDevice;
}
