/*
MIT License

Copyright (c) 2017-2019 Benjamin "Nefarius" H�glinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * TODO:
 * Error checking
 * Vibration feedback
 * Async plugin
 */

 // WinAPI
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Dwmapi.h>
#include "resource.h"

// ImGui + SFML helper
#include "imgui.h"
#include "imgui-SFML.h"

// SFML
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

// I/O
#include <Xinput.h>
#include <ViGEm/Client.h>

// STL
#include <string>
#include <iosfwd>
#include <sstream>
#include <array>

// Internals
#include "VDX.h"


static std::array<EmulationTarget, XUSER_MAX_COUNT> g_targets;
static XInputSetState_t g_pXInputSetState = nullptr;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	auto vm = sf::VideoMode(550, 140);
	sf::RenderWindow window(vm, "XInput to ViGEm proxy application", sf::Style::None);
	// NOTE: this is also the controller polling frequency, up it if unsatisfied ;)
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	//apply_imgui_style();

	// Enable window transparency
	MARGINS margins;
	margins.cxLeftWidth = -1;
	SetWindowLong(window.getSystemHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
	DwmExtendFrameIntoClientArea(window.getSystemHandle(), &margins);

	// Set window icon
	auto hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	if (hIcon)
	{
		SendMessage(window.getSystemHandle(), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
	}

	// Alloc ViGEm client
	const auto client = vigem_alloc();

	// Dynamically load XInput1_3.dll provided by x360ce or system
	const auto xInputMod = LoadLibrary(L"XInput1_3.dll");

	if (!xInputMod)
	{
		MessageBox(window.getSystemHandle(), L"XInput1_3.dll missing", L"Error", MB_ICONERROR);
		return -1;
	}

	// Declare XInput functions  
	const auto pXInputEnable = reinterpret_cast<VOID(WINAPI*)(BOOL)>(GetProcAddress(xInputMod, "XInputEnable"));
	const auto pXInputGetState = reinterpret_cast<DWORD(WINAPI*)(DWORD, XINPUT_STATE*)>(GetProcAddress(
		xInputMod, "XInputGetState"));
	g_pXInputSetState = reinterpret_cast<XInputSetState_t>(GetProcAddress(xInputMod, "XInputSetState"));
	const auto pXInputGetCapabilities = reinterpret_cast<DWORD(WINAPI*)(DWORD, DWORD, XINPUT_CAPABILITIES*)>(
		GetProcAddress(xInputMod, "XInputGetCapabilities"));
	/*
	 * https://forums.tigsource.com/index.php?&topic=26792.msg847843#msg847843
	 * https://github.com/DieKatzchen/GuideButtonPoller/blob/master/GuideButtonPoller.cpp
	 * http://reverseengineerlog.blogspot.co.at/2016/06/xinputs-hidden-functions.html
	 */
	const auto pXInputGetStateSecret = reinterpret_cast<int(__stdcall*)(int, XINPUT_GAMEPAD_SECRET*)>(GetProcAddress(
		xInputMod, (LPCSTR)100));
	if (pXInputGetStateSecret == nullptr)
	{
		MessageBox(window.getSystemHandle(), L"XBOX Guide button not readable", L"Warning", MB_ICONWARNING);
	}

	const auto retval = vigem_connect(client);
	if (!VIGEM_SUCCESS(retval))
	{
		std::wstringstream stream;
		stream << L"ViGEm Bus connection failed with error code: 0x" << std::hex << retval;
		std::wstring result(stream.str());
		MessageBox(window.getSystemHandle(), result.c_str(), L"Error", MB_ICONERROR);
		return -1;
	}

	pXInputEnable(TRUE);

	// Test all input slots on first launch
	for (auto& g_target : g_targets)
	{
		g_target.hasPresenceChanged = true;
	}

	XINPUT_STATE state;
	XINPUT_GAMEPAD_SECRET secret;

	sf::Vector2i grabbedOffset;
	auto grabbedWindow = false;
	auto isOpen = true;
	window.resetGLStates();
	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			// Escape key closes window/application
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
			}
			// Mouse events used to react to dragging
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					grabbedOffset = window.getPosition() - sf::Mouse::getPosition();
					grabbedWindow = true;
				}
			}
			// Mouse events used to react to dragging
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					grabbedWindow = false;
			}
			// Mouse events used to react to dragging
			else if (event.type == sf::Event::MouseMoved)
			{
				if (grabbedWindow)
					window.setPosition(sf::Mouse::getPosition() + grabbedOffset);
			}
			// 
			// Reacts to WM_DEVICECHANGE for better performance
			// 
			// See https://github.com/ocornut/imgui/blob/f0348ddffc41a9ef1e34e930796ab68fe959079e/examples/imgui_impl_win32.cpp#L162-L163
			// 
			else if (event.type == sf::Event::JoystickConnected || event.type == sf::Event::JoystickDisconnected)
			{
				// Joystick ID isn't reliable with XInput devices, so re-scan all
				for (auto& g_target : g_targets)
				{
					g_target.hasPresenceChanged = true;
				}
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		// Create main window
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(vm.width * 1.0f, vm.height * 1.0f));
		ImGui::Begin("XInput to ViGEm proxy application", &isOpen,
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoScrollbar);

		if (!isOpen) break;

		ImGui::Columns(4);

		// Header
		ImGui::Text("Player index");
		ImGui::NextColumn();
		ImGui::Text("Status");
		ImGui::SetColumnOffset(-1, 100);
		ImGui::NextColumn();
		ImGui::Text("Emulation type");
		ImGui::SetColumnOffset(-1, 200);
		ImGui::NextColumn();
		ImGui::Text("Action");
		ImGui::SetColumnOffset(-1, 450);
		ImGui::NextColumn();

		ImGui::Separator();

		// Build main controls
		for (auto i = 0; i < XUSER_MAX_COUNT; i++)
		{
			auto& pad = g_targets[i];

			ImGui::Text("Player %d", i + 1);
			ImGui::NextColumn();

			// Pad for this player index had been added or removed, test if available
			if (pad.hasPresenceChanged)
			{
				XINPUT_CAPABILITIES caps;
				pad.isSourceConnected = (pXInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
				pad.hasPresenceChanged = false;
			}

			ImGui::Text(pad.isSourceConnected ? "Connected" : "Disconnected");

			ImGui::NextColumn();
			ImGui::PushItemWidth(230);
			ImGui::PushID(i);
			ImGui::Combo("##dummy",
				reinterpret_cast<int*>(&pad.targetType),
				"Xbox 360 Controller\0DualShock 4 Controller\0\0");
			ImGui::PopID();
			ImGui::NextColumn();

			// Only call expensive API to grab report if pad is present
			if (pad.isSourceConnected && (pXInputGetState(i, &state) == ERROR_SUCCESS))
			{
				ImGui::PushID(i);

				const auto clicked = ImGui::Button(pad.isTargetConnected ? "Disconnect" : "Connect");

				if (clicked)
				{
					// Disconnect requested
					if (pad.isTargetConnected)
					{
						// Disconnect target
						pad.isTargetConnected = !VIGEM_SUCCESS(vigem_target_remove(client, pad.target));

						switch (pad.targetType)
						{
						case X360:
							vigem_target_x360_unregister_notification(pad.target);
							break;
						case DS4:
							vigem_target_ds4_unregister_notification(pad.target);
							break;
						}
												
						// Free memory
						vigem_target_free(pad.target);
					}
					else
					{
						// Allocate object depending on requested target type
						switch (pad.targetType)
						{
						case X360:
							pad.target = vigem_target_x360_alloc();
							break;
						case DS4:
							pad.target = vigem_target_ds4_alloc();
							break;
						}

						// Plug in the target
						const auto pir = vigem_target_add(client, pad.target);

						// Crude error handling
						if (!VIGEM_SUCCESS(pir))
						{
							MessageBox(window.getSystemHandle(), L"Target plugin failed", L"Error", MB_ICONERROR);
							return -1;
						}

						// Store user index
						pad.userIndex = i;
						
						// Register rumble feedback handler
						switch (pad.targetType)
						{
						case X360:
							vigem_target_x360_register_notification(client, pad.target, notify_x360, static_cast<LPVOID>(&g_targets[i]));
							break;
						case DS4:
							vigem_target_ds4_register_notification(client, pad.target, notify_ds4, static_cast<LPVOID>(&g_targets[i]));
							break;
						}
						
						// Update state
						pad.isTargetConnected = vigem_target_is_attached(pad.target);
					}
				}

				ImGui::PopID();
			}

			if (pad.isTargetConnected)
			{
				// Uses "secret" API function to listen for Guide button and injects it into 
				// the original XINPUT_GAMEPAD structure if detected
				if (
					(pXInputGetStateSecret != nullptr)
					&& (pXInputGetStateSecret(i, &secret) == ERROR_SUCCESS)
					&& ((secret.wButtons & XUSB_GAMEPAD_GUIDE) != 0)
					)
				{
					state.Gamepad.wButtons |= XUSB_GAMEPAD_GUIDE;
				}

				switch (pad.targetType)
				{
				case X360:

					// The XINPUT_GAMEPAD structure is identical to the XUSB_REPORT structure
					// so we can simply take it "as-is" and cast it.
					vigem_target_x360_update(client, pad.target, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));

					break;
				case DS4:
				{
					DS4_REPORT_EX rep;
					DS4_REPORT_EX_INIT(&rep);

					// The DualShock 4 expects a different report format, so we call a helper 
					// function which will translate buttons and axes 1:1 from XUSB to DS4
					// format and submit it to the update function afterwards.
					XUSB_TO_DS4_REPORT_EX(reinterpret_cast<PXUSB_REPORT>(&state.Gamepad), &rep);

					vigem_target_ds4_update_ex(client, pad.target, rep);
				}
				break;
				}
			}

			ImGui::NextColumn();
		}

		ImGui::End();

		window.clear(sf::Color::Transparent);

		ImGui::SFML::Render(window);
		window.display();
	}

	// Clean-up
	pXInputEnable(FALSE);
	vigem_disconnect(client);
	vigem_free(client);

	ImGui::SFML::Shutdown();

	return 0;
}

//
// Invoked when rumble requests come in for x360 target
// 
VOID CALLBACK notify_x360(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber,
	LPVOID UserData
)
{
	const auto pad = static_cast<EmulationTarget*>(UserData);

	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = LargeMotor * 257;
	vibration.wRightMotorSpeed = SmallMotor * 257;

	g_pXInputSetState(pad->userIndex, &vibration);
}

//
// Invoked when rumble requests come in for DS4 target
// 
VOID CALLBACK notify_ds4(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	DS4_LIGHTBAR_COLOR LightbarColor,
	LPVOID UserData)
{
	const auto pad = static_cast<EmulationTarget*>(UserData);

	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = LargeMotor * 257;
	vibration.wRightMotorSpeed = SmallMotor * 257;

	g_pXInputSetState(pad->userIndex, &vibration);
}
