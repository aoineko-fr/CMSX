//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Joystick handler using direct access to PSG port
//-----------------------------------------------------------------------------
#include "core.h"
#include "ports.h"
#include "joystick.h"

//-----------------------------------------------------------------------------
// Simple access functions
//-----------------------------------------------------------------------------

#define PSG_REG_IO_PORT_A	14
#define PSG_REG_IO_PORT_B	15

//-----------------------------------------------------------------------------
// Get the current joystick information (bit=0: pressed)
// Input  : JOY_PORT_1 or JOY_PORT_2
// Output : xxBARLDU
//            │││││└─ Up
//            ││││└── Down
//            │││└─── Left
//            ││└──── Right
//            │└───── Trigger A
//            └────── Trigger B
u8 Joystick_Read(u8 port) __FASTCALL
{
	port;
	// FastCall
	//	ld		l, port
	__asm
		ld		a, #PSG_REG_IO_PORT_B
		out		(P_PSG_REGS), a				// Select port B
		in		a, (P_PSG_STAT)				// Read port B value
		res		6, a
		or		a, l
		out		(P_PSG_DATA), a				// Write port B value
		ld		a, #PSG_REG_IO_PORT_A
		out		(P_PSG_REGS), a				// Select port A
		in		a, (P_PSG_STAT)				// Read port A value
		ld		l, a						// Return value
	__endasm;		
}

//-----------------------------------------------------------------------------
// Get current direction of the given joystick
// Input  : JOY_PORT_1 or JOY_PORT_2
inline u8 Joystick_GetDirection(u8 port)
{
	u8 in = Joystick_Read(port);
	in = ~in;
	in &= JOY_INPUT_DIR_MASK;
	return in;
}

//-----------------------------------------------------------------------------
// Get current trigger status of the given joystick (0: released; 1: pressed)
// Input  : JOY_PORT_1 or JOY_PORT_2
//          JOY_INPUT_TRIGGER_A or JOY_INPUT_TRIGGER_B
inline u8 Joystick_GetTrigger(u8 port, u8 trigger)
{
	u8 in = Joystick_Read(port);
	return ((in & trigger) == 0);
}

//-----------------------------------------------------------------------------
// Complete joystick manager
//-----------------------------------------------------------------------------
#if USE_JOYSTICK_MANAGER

// JSM_ALLOC_DATA()
// #define JSM_ALLOC_DATA() JSM_Data g_JSM;
JSM_Data g_JSM;

//-----------------------------------------------------------------------------
u8 CheckClick(u8 joy, u8 in)
{
	if((g_JSM.JoyData[joy].State[in] == JSM_STATE_ON) && (g_JSM.JoyData[joy].Timer[in] == 0))
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckRelease(u8 joy, u8 in)
{
	if((g_JSM.JoyData[joy].State[in] == JSM_STATE_OFF) && (g_JSM.JoyData[joy].Timer[in] == 0))
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckHold(u8 joy, u8 in)
{
	if((g_JSM.JoyData[joy].State[in] == JSM_STATE_ON) && (g_JSM.JoyData[joy].Timer[in] == g_JSM.Config.HoldTimer))
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckHoldRelease(u8 joy, u8 in)
{
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckDoubleClick(u8 joy, u8 in)
{
	if((g_JSM.JoyData[joy].State[in] == JSM_STATE_ON) && (g_JSM.JoyData[joy].Timer[in] == 0) && (g_JSM.JoyData[joy].PrevTimer[in] <= g_JSM.Config.DoubleClickTimer))
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckDoubleClickRelease(u8 joy, u8 in)
{
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckDoubleClickHold(u8 joy, u8 in)
{
	return 0;
}

//-----------------------------------------------------------------------------
u8 CheckDOUBLEClickHoldRelease(u8 joy, u8 in)
{
	return 0;
}

//-----------------------------------------------------------------------------
// Initialize Joystick manager
void JSM_Initialize()
{
	for(i8 joy = 0; joy < 4; joy++)
	{
		g_JSM.JoyData[joy].CurrentStatus  = 0xFF;
		g_JSM.JoyData[joy].PreviousStatus = 0xFF;
		
		for(i8 in = 0; in < JSM_INPUT_MAX; in++)
		{
			g_JSM.JoyData[joy].State[in] = JSM_STATE_OFF;
			g_JSM.JoyData[joy].Timer[in] = 0xFF;
			g_JSM.JoyData[joy].PrevState[in] = JSM_STATE_OFF;
			g_JSM.JoyData[joy].PrevTimer[in] = 0xFF;
		}
	}
	
	g_JSM.EventsNum = 0;
	g_JSM.Checker[JSM_EVENT_CLICK]                     = CheckClick;
	g_JSM.Checker[JSM_EVENT_RELEASE]                   = CheckRelease;
	g_JSM.Checker[JSM_EVENT_HOLD]                      = CheckHold;
	g_JSM.Checker[JSM_EVENT_HOLD_RELEASE]              = CheckHoldRelease;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK]              = CheckDoubleClick;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK_RELEASE]      = CheckDoubleClickRelease;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK_HOLD]         = CheckDoubleClickHold;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK_HOLD_RELEASE] = CheckDOUBLEClickHoldRelease;

	g_JSM.Config.HoldTimer        = 0x10;
	g_JSM.Config.DoubleClickTimer = 0x04;
}

//-----------------------------------------------------------------------------
// Update Joystick manager
void JSM_Update()
{
	// Update status
	for(i8 joy = 0; joy < 2; joy++)
	{
		JSM_System* data = &g_JSM.JoyData[joy];
		data->PreviousStatus = data->CurrentStatus;
		data->CurrentStatus = Joystick_Read((joy == 0) ? JOY_PORT_1 : JOY_PORT_2);

		for(i8 in = 0; in < JSM_INPUT_MAX; in++)
		{
			if(data->Timer[in] < 0xFF)
				data->Timer[in]++;
		}

		u8 cur = data->CurrentStatus;
		u8 prev = data->PreviousStatus;

		// Check Trigger A state change
		if(((cur & JOY_INPUT_TRIGGER_A) == 0) && ((prev & JOY_INPUT_TRIGGER_A) != 0))
		{
			data->PrevState[JSM_INPUT_BUTTON_A] = data->State[JSM_INPUT_BUTTON_A];
			data->PrevTimer[JSM_INPUT_BUTTON_A] = data->Timer[JSM_INPUT_BUTTON_A];
			data->State[JSM_INPUT_BUTTON_A] = JSM_STATE_ON;
			data->Timer[JSM_INPUT_BUTTON_A] = 0;
		}
		else if(((cur & JOY_INPUT_TRIGGER_A) != 0) && ((prev & JOY_INPUT_TRIGGER_A) == 0))
		{
			data->PrevState[JSM_INPUT_BUTTON_A] = data->State[JSM_INPUT_BUTTON_A];
			data->PrevTimer[JSM_INPUT_BUTTON_A] = data->Timer[JSM_INPUT_BUTTON_A];
			data->State[JSM_INPUT_BUTTON_A] = JSM_STATE_OFF;
			data->Timer[JSM_INPUT_BUTTON_A] = 0;
		}

		// Check Trigger A state change
		if(((cur & JOY_INPUT_TRIGGER_B) == 0) && ((prev & JOY_INPUT_TRIGGER_B) != 0))
		{
			data->PrevState[JSM_INPUT_BUTTON_B] = data->State[JSM_INPUT_BUTTON_B];
			data->PrevTimer[JSM_INPUT_BUTTON_B] = data->Timer[JSM_INPUT_BUTTON_B];
			data->State[JSM_INPUT_BUTTON_B] = JSM_STATE_ON;
			data->Timer[JSM_INPUT_BUTTON_B] = 0;
		}
		else if(((cur & JOY_INPUT_TRIGGER_B) != 0) && ((prev & JOY_INPUT_TRIGGER_B) == 0))
		{
			data->PrevState[JSM_INPUT_BUTTON_B] = data->State[JSM_INPUT_BUTTON_B];
			data->PrevTimer[JSM_INPUT_BUTTON_B] = data->Timer[JSM_INPUT_BUTTON_B];
			data->State[JSM_INPUT_BUTTON_B] = JSM_STATE_OFF;
			data->Timer[JSM_INPUT_BUTTON_B] = 0;
		}

		// Check stick state change
		if(((cur & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK) && ((prev & JOY_INPUT_DIR_MASK) == JOY_INPUT_DIR_MASK))
		{
			data->PrevState[JSM_INPUT_STICK] = data->State[JSM_INPUT_STICK];
			data->PrevTimer[JSM_INPUT_STICK] = data->Timer[JSM_INPUT_STICK];
			data->State[JSM_INPUT_STICK] = JSM_STATE_ON;
			data->Timer[JSM_INPUT_STICK] = 0;
		}
		else if(((cur & JOY_INPUT_DIR_MASK) == JOY_INPUT_DIR_MASK) && ((prev & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK))
		{
			data->PrevState[JSM_INPUT_STICK] = data->State[JSM_INPUT_STICK];
			data->PrevTimer[JSM_INPUT_STICK] = data->Timer[JSM_INPUT_STICK];
			data->State[JSM_INPUT_STICK] = JSM_STATE_OFF;
			data->Timer[JSM_INPUT_STICK] = 0;
		}
	}
	
	// Check registered events
	for(i8 i = 0; i < g_JSM.EventsNum; i++)
	{
		JSM_Entry* entry = &g_JSM.Events[i];
		
		jsm_cb cb = entry->Callback;
		if(cb == null)
			continue;

		if(entry->Event.JoyId < JSM_JOYSTICK_MAX)
		{	
			u8 joy = entry->Event.JoyId;
			if(entry->Event.EventId < JSM_EVENT_MAX)
			{
				u8 ev = entry->Event.EventId;
				if(entry->Event.InputId < JSM_INPUT_MAX)
				{
					u8 in = entry->Event.InputId;
					if(g_JSM.Checker[ev](joy, in))
						cb(&entry->Event);
				}
				else // JSM_INPUT_ANY
				{
					for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
						if(g_JSM.Checker[ev](joy, in))
							cb(&entry->Event);			
				}
			}
			else // JSM_EVENT_ANY
			{
				for(i8 ev = 0; ev < JSM_EVENT_MAX; ev++)
				{
					if(entry->Event.InputId < JSM_INPUT_MAX)
					{
						u8 in = entry->Event.InputId;
						if(g_JSM.Checker[ev](joy, in))
							cb(&entry->Event);
					}
					else
					{
						for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
							if(g_JSM.Checker[ev](joy, in))
								cb(&entry->Event);			
					}
				}
			}
		}	
		else // JSM_JOYSTICK_ANY
		{	
			for(i8 joy = 0; joy < JSM_JOYSTICK_MAX; joy++)
			{
				if(entry->Event.EventId < JSM_EVENT_MAX)
				{
					u8 ev = entry->Event.EventId;
					if(entry->Event.InputId < JSM_INPUT_MAX)
					{
						u8 in = entry->Event.InputId;
						if(g_JSM.Checker[ev](joy, in))
							cb(&entry->Event);
					}
					else // JSM_INPUT_ANY
					{
						for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
							if(g_JSM.Checker[ev](joy, in))
								cb(&entry->Event);			
					}
				}
				else // JSM_EVENT_ANY
				{
					for(i8 ev = 0; ev < JSM_EVENT_MAX; ev++)
					{
						if(entry->Event.InputId < JSM_INPUT_MAX)
						{
							u8 in = entry->Event.InputId;
							if(g_JSM.Checker[ev](joy, in))
								cb(&entry->Event);
						}
						else
						{
							for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
								if(g_JSM.Checker[ev](joy, in))
									cb(&entry->Event);			
						}
					}
				}
			}
		}	
	}
}

//-----------------------------------------------------------------------------
// Register a callback to a given Joystick manager's event
void JSM_RegisterEvent(u8 id, u8 input, u8 event, u8 userdata, jsm_cb cb)
{
	if(g_JSM.EventsNum < JSM_EVENT_TAB_SIZE - 1)
	{
		u8 i = g_JSM.EventsNum++;
		g_JSM.Events[i].Event.JoyId = id;
		g_JSM.Events[i].Event.InputId = input;
		g_JSM.Events[i].Event.EventId = event;
		g_JSM.Events[i].Event.UserData = userdata;
		g_JSM.Events[i].Callback = cb;
	}
}

//-----------------------------------------------------------------------------
// Get current joystick status
// Input  : JSM_JOYSTICK
// Output : 
inline u8 JSM_GetStatus(u8 joy)
{
	return g_JSM.JoyData[joy].CurrentStatus;
}

//-----------------------------------------------------------------------------
// Get current direction of the given joystick
inline i8 JSM_GetStickDirection(u8 joy)
{
	u8 in = g_JSM.JoyData[joy].CurrentStatus;
	in = ~in;
	in &= JOY_INPUT_DIR_MASK;
	switch(in)
	{
	case JOY_INPUT_DIR_UP:			return 0;
	case JOY_INPUT_DIR_UP_RIGTH:	return 1;
	case JOY_INPUT_DIR_RIGHT:		return 2;
	case JOY_INPUT_DIR_DOWN_RIGTH:	return 3;
	case JOY_INPUT_DIR_DOWN:		return 4;
	case JOY_INPUT_DIR_DOWN_LEFT:	return 5;
	case JOY_INPUT_DIR_LEFT:		return 6;
	case JOY_INPUT_DIR_UP_LEFT:		return 7;
	default:						break;
	}	
	
	return -1;
}

//-----------------------------------------------------------------------------
// Get current joystick state
inline u8 JSM_GetInputState(u8 joy, u8 in)
{
	return g_JSM.JoyData[joy].State[in];
}

//-----------------------------------------------------------------------------
// Get current joystick state timer
inline u8 JSM_GetInputTimer(u8 joy, u8 in)
{
	return g_JSM.JoyData[joy].Timer[in];
}

//-----------------------------------------------------------------------------
//
const c8* JSM_GetEventName(u8 ev)
{
	switch(ev)
	{
	case JSM_EVENT_CLICK:
		return "C   ";
	case JSM_EVENT_RELEASE:
		return "R  ";
	case JSM_EVENT_HOLD:
		return "H  ";
	case JSM_EVENT_HOLD_RELEASE:
		return "HR ";
	case JSM_EVENT_DOUBLE_CLICK:
		return "DC ";
	case JSM_EVENT_DOUBLE_CLICK_RELEASE:
		return "DCR ";
	case JSM_EVENT_DOUBLE_CLICK_HOLD:
		return "DCH ";
	case JSM_EVENT_DOUBLE_CLICK_HOLD_RELEASE:
		return "DCHR";
	default:
		break;
	}
	return "????";
}

#endif // USE_JOYSTICK_MANAGER
