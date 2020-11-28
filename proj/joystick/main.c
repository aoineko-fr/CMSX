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
#include "joystick.h"

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

#define LOG_MAX 7
const JSM_Event* g_EventLog[2][LOG_MAX];
u8 g_EventNum[2];

void JoystickEvent(const JSM_Event* event)
{
	if(g_EventNum[event->JoyId] > 0)
	{
		for(i8 i = g_EventNum[event->JoyId]; i > 0 ; i--)
			g_EventLog[event->JoyId][i] = g_EventLog[event->JoyId][i-1];
	}
	g_EventLog[event->JoyId][0] = event;
	if(g_EventNum[event->JoyId] < LOG_MAX - 1)
		g_EventNum[event->JoyId]++;
	
	for(i8 i = 0; i < g_EventNum[event->JoyId]; i++)
	{
		SetPrintPos(3 + (21 * event->JoyId), 18 + i);
		PrintText("      ");
		SetPrintPos(3 + (21 * event->JoyId), 18 + i);
		switch(g_EventLog[event->JoyId][i]->InputId)
		{
		case JSM_INPUT_BUTTON_A: PrintText("A:"); break;
		case JSM_INPUT_BUTTON_B: PrintText("B:"); break;
		case JSM_INPUT_STICK:    PrintText("S:"); break;
		};
		PrintText(JSM_GetEventName(g_EventLog[event->JoyId][i]->EventId));
	}
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

	g_EventNum[0] = g_EventNum[1] = 0;

	JSM_Initialize();
	JSM_RegisterEvent(JSM_JOYSTICK_1, JSM_INPUT_ANY, JSM_EVENT_ANY, 0, JoystickEvent);

	PrintBox(1, 1, 40, 3);

	SetPrintPos(3, 2);
	PrintText("JOYSTICK SAMPLE");

	PrintLineX(1, 7, 40);
	SetPrintPos(3, 7);
	PrintText(" Raw access ");

	PrintLineX(1, 13, 40);
	SetPrintPos(3, 13);
	PrintText(" JS Manager ");
	
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
		// Events
		SetPrintPos(1 + (21 * joy), 17);
		PrintText("\x01\x47""Events:");
	}

	u8 timer;
	
	while(1)
	{
		JSM_Update();
		
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
			PrintHex8(JSM_GetStickDirection(joy));
			SetPrintPos(16 + (21 * joy), 14);
			timer = JSM_GetInputTimer(joy, JSM_INPUT_STICK);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
			// Button A
			SetPrintPos(12 + (21 * joy), 15);
			PrintText(JSM_GetInputState(joy, JSM_INPUT_BUTTON_A) ? "ON " : "OFF");
			SetPrintPos(16 + (21 * joy), 15);
			timer = JSM_GetInputTimer(joy, JSM_INPUT_BUTTON_A);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
			// Button B
			SetPrintPos(12 + (21 * joy), 16);
			PrintText(JSM_GetInputState(joy, JSM_INPUT_BUTTON_B) ? "ON " : "OFF");
			SetPrintPos(16 + (21 * joy), 16);
			timer = JSM_GetInputTimer(joy, JSM_INPUT_BUTTON_B);
			PrintInt(timer);
			if(timer < 100) PrintChar(' ');
			if(timer < 10) PrintChar(' ');
		}

		VDP_WaitRetrace();
	}
}