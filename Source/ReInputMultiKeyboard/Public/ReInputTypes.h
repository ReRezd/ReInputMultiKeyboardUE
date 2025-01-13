#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.h"
#include "InputCoreTypes.h"
#include "ReInputTypes.generated.h"

UENUM()
enum class EKeyboardKeyState : uint8
{
	UP,
	DOWN,
	HELD
};

UENUM()
enum class EKeyboardDeviceState : uint8
{
	CONNECTED,
	ACTIVATED,
};

USTRUCT()
struct REINPUTMULTIKEYBOARD_API FMultiKeyboardDeviceData
{
	GENERATED_BODY()
	int Index;
	FString ID;
	FText FriendlyName;
	EKeyboardDeviceState State = EKeyboardDeviceState::CONNECTED;
	TArray<FKey> KeyStates;

	FInputDeviceId DeviceInternalID;

	FMultiKeyboardDeviceData();
	FMultiKeyboardDeviceData(int InIndex, FString InID, FText InFriendlyName, EKeyboardDeviceState InState);

	bool IsActive() const { return State == EKeyboardDeviceState::ACTIVATED; }
	void Activate();
};

inline static TMap<USHORT, FKey> VKeyToMKBKey =
	{
		{0x0, EKeys::AnyKey},
		{0x08, EKeys::BackSpace},
		{0x09, EKeys::Tab},
		{0x0D, EKeys::Enter},
		{0x13, EKeys::Pause},
		{0x14, EKeys::CapsLock},
		{0x1B, EKeys::Escape},
		{0x20, EKeys::SpaceBar},
		{0x21, EKeys::PageUp},
		{0x22, EKeys::PageDown},
		{0x23, EKeys::End},
		{0x24, EKeys::Home},
		{0x25, EKeys::Left},
		{0x26, EKeys::Up},
		{0x27, EKeys::Right},
		{0x28, EKeys::Down},
		{0x2D, EKeys::Insert},
		{0x2E, EKeys::Delete},
		{0x30, EKeys::Zero},
		{0x31, EKeys::One},
		{0x32, EKeys::Two},
		{0x33, EKeys::Three},
		{0x34, EKeys::Four},
		{0x35, EKeys::Five},
		{0x36, EKeys::Six},
		{0x37, EKeys::Seven},
		{0x38, EKeys::Eight},
		{0x39, EKeys::Nine},
		{0x41, EKeys::A},
		{0x42, EKeys::B},
		{0x43, EKeys::C},
		{0x44, EKeys::D},
		{0x45, EKeys::E},
		{0x46, EKeys::F},
		{0x47, EKeys::G},
		{0x48, EKeys::H},
		{0x49, EKeys::I},
		{0x4A, EKeys::J},
		{0x4B, EKeys::K},
		{0x4C, EKeys::L},
		{0x4D, EKeys::M},
		{0x4E, EKeys::N},
		{0x4F, EKeys::O},
		{0x50, EKeys::P},
		{0x51, EKeys::Q},
		{0x52, EKeys::R},
		{0x53, EKeys::S},
		{0x54, EKeys::T},
		{0x55, EKeys::U},
		{0x56, EKeys::V},
		{0x57, EKeys::W},
		{0x58, EKeys::X},
		{0x59, EKeys::Y},
		{0x5A, EKeys::Z},
		{0x60, EKeys::NumPadZero},
		{0x61, EKeys::NumPadOne},
		{0x62, EKeys::NumPadTwo},
		{0x63, EKeys::NumPadThree},
		{0x64, EKeys::NumPadFour},
		{0x65, EKeys::NumPadFive},
		{0x66, EKeys::NumPadSix},
		{0x67, EKeys::NumPadSeven},
		{0x68, EKeys::NumPadEight},
		{0x69, EKeys::NumPadNine},
		{0x6A, EKeys::Multiply},
		{0x6B, EKeys::Add},
		{0x6D, EKeys::Subtract},
		{0x6E, EKeys::Decimal},
		{0x6F, EKeys::Divide},
		{0x70, EKeys::F1},
		{0x71, EKeys::F2},
		{0x72, EKeys::F3},
		{0x73, EKeys::F4},
		{0x74, EKeys::F5},
		{0x75, EKeys::F6},
		{0x76, EKeys::F7},
		{0x77, EKeys::F8},
		{0x78, EKeys::F9},
		{0x79, EKeys::F10},
		{0x7A, EKeys::F11},
		{0x7B, EKeys::F12},
		{0x90, EKeys::NumLock},
		{0x91, EKeys::ScrollLock},
		{0xA0, EKeys::LeftShift},
		{0xA1, EKeys::RightShift},
		{0xA2, EKeys::LeftControl},
		{0xA3, EKeys::RightControl},
		{0xA4, EKeys::LeftAlt},
		{0xA5, EKeys::RightAlt},
		{0x5B, EKeys::LeftCommand},
		{0x5C, EKeys::RightCommand},
		{0xBA, EKeys::Semicolon},
		{0xBB, EKeys::Equals},
		{0xBC, EKeys::Comma},
		{0xBD, EKeys::Hyphen},
		{0xBE, EKeys::Period},
		{0xBF, EKeys::Slash},
		{0xC0, EKeys::Tilde},
		{0xDB, EKeys::LeftBracket},
		{0xDC, EKeys::Backslash},
		{0xDD, EKeys::RightBracket},
		{0xDE, EKeys::Apostrophe},
	};