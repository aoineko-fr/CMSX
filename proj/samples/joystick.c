//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Joystick sample program
//-----------------------------------------------------------------------------

#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "video.h"
#include "bios_main.h"
#include "print.h"
#include "input.h"

//=============================================================================
//
//   C O D E
//
//=============================================================================

void MainLoop();

//-----------------------------------------------------------------------------
/** Program entry point */
void main()
{
	MainLoop();
}

// Log
u8 LogX, LogY;

void JoystickEvent(u8 joy, u8 in, u8 evt)
{
	if(LogY >= 24)
	{
		for(i8 i = 18; i < 24; i++) // clear
		{
			SetPrintPos(1, i);
			PrintCharX(' ', 40);
		}
		LogX = 1;
		LogY = 18;
	}

	SetPrintPos(LogX, LogY);
	PrintChar('J');
	PrintInt(joy);
	PrintChar('.');
	switch(in)
	{
	case IPM_INPUT_STICK:    PrintText("S"); break;
	case IPM_INPUT_BUTTON_A: PrintText("A"); break;
	case IPM_INPUT_BUTTON_B: PrintText("B"); break;
	}
	PrintChar(':');
	PrintText(IPM_GetEventName(evt));
	PrintChar(' ');
	
	LogX = g_CSRX;
	LogY = g_CSRY;

}


//-----------------------------------------------------------------------------
/** Main loop */
void MainLoop()
{
	Bios_Beep();
	
	g_LINL40 = 40;
	Bios_ChangeMode(SCREEN_0);
	Bios_ChangeColor(COLOR_WHITE, COLOR_DARK_BLUE, COLOR_DARK_BLUE);
	Bios_ClearScreen();

	LogX = 1;
	LogY = 18;

	IPM_Initialize(null);
	IPM_RegisterEvent(IPM_DEVICE_JOYSTICK_1, IPM_INPUT_ANY, IPM_EVENT_CLICK, JoystickEvent);
	IPM_RegisterEvent(IPM_DEVICE_JOYSTICK_1, IPM_INPUT_ANY, IPM_EVENT_HOLD, JoystickEvent);
	IPM_RegisterEvent(IPM_DEVICE_JOYSTICK_1, IPM_INPUT_ANY, IPM_EVENT_DOUBLE_CLICK, JoystickEvent);
	IPM_RegisterEvent(IPM_DEVICE_JOYSTICK_1, IPM_INPUT_ANY, IPM_EVENT_DOUBLE_CLICK_HOLD, JoystickEvent);

	PrintBox(1, 1, 40, 3);

	SetPrintPos(3, 2);
	PrintText("JOYSTICK SAMPLE");

	PrintLineX(1, 7, 40);
	SetPrintPos(3, 7);
	PrintText(" Raw access ");

	PrintLineX(1, 13, 40);
	SetPrintPos(3, 13);
	PrintText(" JS Manager ");

	// Events
	SetPrintPos(1, 17);
	PrintText("\x01\x47""Events log:");
	
	for(i8 joy = 0; joy < 2; joy++)
	{
		// ID
		SetPrintPos(1 + (21 * joy), 5);
		PrintText("Port#");
		PrintInt(joy);

		// Raw access
		// Status
		SetPrintPos(1 + (21 * joy), 8);
		PrintText("\x01\x47""Status");
		// Direction
		SetPrintPos(1 + (21 * joy), 9);
		PrintText("\x01\x47""Direction");
		// Trigger A
		SetPrintPos(1 + (21 * joy), 10);
		PrintText("\x01\x47""Trig A");
		// Trigger B
		SetPrintPos(1 + (21 * joy), 11);
		PrintText("\x01\x47""Trig B");

		// Joystick Manager
		// Stick
		SetPrintPos(1 + (21 * joy), 14);
		PrintText("\x01\x47""Stick");
		// Button A
		SetPrintPos(1 + (21 * joy), 15);
		PrintText("\x01\x47""Button A");
		// Button B
		SetPrintPos(1 + (21 * joy), 16);
		PrintText("\x01\x47""Button B");
	}

	u8 timer;
	
	while(1)
	{
		IPM_Update();
		
		for(i8 joy = 0; joy < 2; joy++)
		{
			// Raw access
			
			// Status
			SetPrintPos(12 + (21 * joy), 8);
			PrintHex8(Joystick_Read((joy == 0) ? JOY_PORT_1 : JOY_PORT_2));
			// Direction
			SetPrintPos(12 + (21 * joy), 9);
			u8 dir = Joystick_GetDirection((joy == 0) ? JOY_PORT_1 : JOY_PORT_2);
			PrintInt(dir);
			PrintChar(' ');
			// Trigger A
			SetPrintPos(12 + (21 * joy), 10);
			PrintInt(Joystick_GetTrigger((joy == 0) ? JOY_PORT_1 : JOY_PORT_2, JOY_INPUT_TRIGGER_A));
			// Trigger B
			SetPrintPos(12 + (21 * joy), 11);
			PrintInt(Joystick_GetTrigger((joy == 0) ? JOY_PORT_1 : JOY_PORT_2, JOY_INPUT_TRIGGER_B));

			// Joystick
			SetPrintPos(16 + (21 * joy), 9);
			PrintText("   ");		
			SetPrintPos(16 + (21 * joy), 10);
			PrintText(" O ");		
			SetPrintPos(16 + (21 * joy), 11);
			PrintText("   ");
			switch(dir)
			{
			case JOY_INPUT_DIR_UP:
				SetPrintPos(17 + (21 * joy), 9);
				PrintChar('|');
				break;
			case JOY_INPUT_DIR_DOWN:
				SetPrintPos(17 + (21 * joy), 11);
				PrintChar('|');
				break;
			case JOY_INPUT_DIR_LEFT:
				SetPrintPos(16 + (21 * joy), 10);
				PrintChar('-');
				break;
			case JOY_INPUT_DIR_RIGHT:
				SetPrintPos(18 + (21 * joy), 10);
				PrintChar('-');
				break;
			case JOY_INPUT_DIR_UP_RIGTH:
				SetPrintPos(18 + (21 * joy), 9);
				PrintChar('/');
				break;
			case JOY_INPUT_DIR_UP_LEFT:
				SetPrintPos(16 + (21 * joy), 9);
				PrintChar('\\');
				break;
			case JOY_INPUT_DIR_DOWN_RIGTH:
				SetPrintPos(18 + (21 * joy), 11);
				PrintChar('\\');
				break;
			case JOY_INPUT_DIR_DOWN_LEFT:
				SetPrintPos(16 + (21 * joy), 11);
				PrintChar('/');
				break;
			}

			// Stick
			SetPrintPos(12 + (21 * joy), 14);
			PrintHex8(IPM_GetStickDirection(joy));
			SetPrintPos(16 + (21 * joy), 14);
			timer = IPM_GetInputTimer(joy, IPM_INPUT_STICK);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
			// Button A
			SetPrintPos(12 + (21 * joy), 15);
			switch(IPM_GetInputState(joy, IPM_INPUT_BUTTON_A) & IPM_STATE_PRESSMASK)
			{
			case IPM_STATE_OFF:     PrintText("OFF"); break;
			case IPM_STATE_PRESS:   PrintText("PRE"); break;
			case IPM_STATE_ON:      PrintText("ON "); break;
			case IPM_STATE_RELEASE: PrintText("REL"); break;
			}
			SetPrintPos(16 + (21 * joy), 15);
			timer = IPM_GetInputTimer(joy, IPM_INPUT_BUTTON_A);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
			// Button B
			SetPrintPos(12 + (21 * joy), 16);
			PrintText(IPM_GetInputState(joy, IPM_INPUT_BUTTON_B) ? "ON " : "OFF");
			SetPrintPos(16 + (21 * joy), 16);
			timer = IPM_GetInputTimer(joy, IPM_INPUT_BUTTON_B);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
		}

		VDP_WaitRetrace();
	}
}