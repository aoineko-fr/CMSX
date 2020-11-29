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

JSM_Data g_JSM;

//-----------------------------------------------------------------------------
u8 CheckClick(u8 joy, u8 in)
{
	return (g_JSM.Process[joy].State[in] == JSM_STATE_PRESS);
}

//-----------------------------------------------------------------------------
u8 CheckRelease(u8 joy, u8 in)
{
	return (g_JSM.Process[joy].State[in] == JSM_STATE_RELEASE);
}

//-----------------------------------------------------------------------------
u8 CheckHold(u8 joy, u8 in)
{
	return (g_JSM.Process[joy].State[in] == (JSM_STATE_ON + JSM_STATE_HOLD));
}

//-----------------------------------------------------------------------------
u8 CheckDoubleClick(u8 joy, u8 in)
{
	return (g_JSM.Process[joy].State[in] == (JSM_STATE_PRESS + JSM_STATE_DOUBLE));
}

//-----------------------------------------------------------------------------
u8 CheckDoubleClickHold(u8 joy, u8 in)
{
	return (g_JSM.Process[joy].State[in] == (JSM_STATE_ON + JSM_STATE_DOUBLE + JSM_STATE_HOLD));
}

//-----------------------------------------------------------------------------
// Initialize Joystick manager
void JSM_Initialize()
{
	for(i8 joy = 0; joy < 4; joy++)
	{
		g_JSM.Process[joy].CurrentStatus  = 0xFF;
		g_JSM.Process[joy].PreviousStatus = 0xFF;
		
		for(i8 in = 0; in < JSM_INPUT_MAX; in++)
		{
			g_JSM.Process[joy].State[in] = 0;
			g_JSM.Process[joy].Timer[in] = 0xFF;
		}
	}
	
	g_JSM.EventsNum = 0;
	g_JSM.Checker[JSM_EVENT_CLICK]             = CheckClick;
	g_JSM.Checker[JSM_EVENT_RELEASE]           = CheckRelease;
	g_JSM.Checker[JSM_EVENT_HOLD]              = CheckHold;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK]      = CheckDoubleClick;
	g_JSM.Checker[JSM_EVENT_DOUBLE_CLICK_HOLD] = CheckDoubleClickHold;

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
		JSM_Process* proc = &g_JSM.Process[joy];
		proc->PreviousStatus = proc->CurrentStatus;
		proc->CurrentStatus = Joystick_Read((joy == 0) ? JOY_PORT_1 : JOY_PORT_2);

		i8 in;
		for(in = 0; in < JSM_INPUT_MAX; in++)
		{
			// Increment previous state counter
			if(proc->Timer[in] < 0xFF)
				proc->Timer[in]++;
			
			// Check state tranition
			if((proc->State[in] & JSM_STATE_PRESSMASK) == JSM_STATE_PRESS) // PRESS >> ON
			{
				proc->State[in] &= ~JSM_STATE_PRESSMASK;
				proc->State[in] |= JSM_STATE_ON;
			}			
			else if((proc->State[in] & JSM_STATE_PRESSMASK) == JSM_STATE_RELEASE) // RELEASE >> OFF
			{
				proc->State[in] &= ~JSM_STATE_PRESSMASK;
				proc->State[in] &= ~JSM_STATE_HOLDMASK;
				proc->State[in] |= JSM_STATE_OFF;
			}
			else if(((proc->State[in] & JSM_STATE_HOLDMASK) == 0) && ((proc->State[in] & JSM_STATE_PRESSMASK) == JSM_STATE_ON) && (proc->Timer[in] > g_JSM.Config.HoldTimer)) // ON >> ON + HOLD
			{
				proc->State[in] |= JSM_STATE_HOLD;
			}
			else if(((proc->State[in] & JSM_STATE_HOLDMASK) == JSM_STATE_HOLD)) // ON + HOLD >> ON + HOLDING
			{
				proc->State[in] &= ~JSM_STATE_HOLDMASK;
				proc->State[in] |= JSM_STATE_HOLDING;
			}
			else if(((proc->State[in] & JSM_STATE_PRESSMASK) == JSM_STATE_OFF) && (proc->Timer[in] > g_JSM.Config.DoubleClickTimer)) // OFF + DBL >> OFF
			{
				proc->State[in] &= ~JSM_STATE_DOUBLE;
			}

			u8 curOn;
			u8 prevOn;			
			if(in == JSM_INPUT_STICK)
			{
				curOn = (proc->CurrentStatus & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK;
				prevOn = (proc->PreviousStatus & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK;
			}
			else
			{
				u8 mask = (in == JSM_INPUT_BUTTON_A) ? JOY_INPUT_TRIGGER_A : JOY_INPUT_TRIGGER_B;
				curOn = (proc->CurrentStatus & mask) == 0;
				prevOn = (proc->PreviousStatus & mask) == 0;
			}

			if(curOn && !prevOn) // OFF >> PRESS
			{
				proc->State[in] &= ~JSM_STATE_PRESSMASK;
				proc->State[in] |= JSM_STATE_PRESS;
				if(proc->Timer[in] <= g_JSM.Config.DoubleClickTimer)
					proc->State[in] |= JSM_STATE_DOUBLE;
				proc->Timer[in] = 0;
			}
			else if(!curOn && prevOn) // ON >> RELEASE
			{
				proc->State[in] = JSM_STATE_RELEASE;
				proc->Timer[in] = 0;
			}
		}
	}
	
	// Check registered events
	for(i8 i = 0; i < g_JSM.EventsNum; i++)
	{
		JSM_Event* entry = &g_JSM.Events[i];
		
		jsm_cb cb = entry->Callback;
		if(cb == null)
			continue;

		if(entry->JoyId < JSM_JOYSTICK_MAX)
		{	
			u8 joy = entry->JoyId;
			if(entry->EventId < JSM_EVENT_MAX)
			{
				u8 ev = entry->EventId;
				if(entry->InputId < JSM_INPUT_MAX)
				{
					u8 in = entry->InputId;
					if(g_JSM.Checker[ev](joy, in))
						cb(joy, in, ev);
				}
				else // JSM_INPUT_ANY
				{
					for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
						if(g_JSM.Checker[ev](joy, in))
							cb(joy, in, ev);			
				}
			}
			else // JSM_EVENT_ANY
			{
				for(i8 ev = 0; ev < JSM_EVENT_MAX; ev++)
				{
					if(entry->InputId < JSM_INPUT_MAX)
					{
						u8 in = entry->InputId;
						if(g_JSM.Checker[ev](joy, in))
							cb(joy, in, ev);
					}
					else // JSM_INPUT_ANY
					{
						for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
							if(g_JSM.Checker[ev](joy, in))
								cb(joy, in, ev);			
					}
				}
			}
		}	
		else // JSM_JOYSTICK_ANY
		{	
			for(i8 joy = 0; joy < JSM_JOYSTICK_MAX; joy++)
			{
				if(entry->EventId < JSM_EVENT_MAX)
				{
					u8 ev = entry->EventId;
					if(entry->InputId < JSM_INPUT_MAX)
					{
						u8 in = entry->InputId;
						if(g_JSM.Checker[ev](joy, in))
							cb(joy, in, ev);
					}
					else // JSM_INPUT_ANY
					{
						for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
							if(g_JSM.Checker[ev](joy, in))
								cb(joy, in, ev);			
					}
				}
				else // JSM_EVENT_ANY
				{
					for(i8 ev = 0; ev < JSM_EVENT_MAX; ev++)
					{
						if(entry->InputId < JSM_INPUT_MAX)
						{
							u8 in = entry->InputId;
							if(g_JSM.Checker[ev](joy, in))
								cb(joy, in, ev);
						}
						else // JSM_INPUT_ANY
						{
							for(i8 in = 0; in < JSM_INPUT_MAX; in ++)
								if(g_JSM.Checker[ev](joy, in))
									cb(joy, in, ev);			
						}
					}
				}
			}
		}	
	}
}

//-----------------------------------------------------------------------------
// Register a callback to a given Joystick manager's event
bool JSM_RegisterEvent(u8 id, u8 input, u8 event, jsm_cb cb)
{
	if(g_JSM.EventsNum < JSM_EVENT_TAB_SIZE - 1)
	{
		u8 i = g_JSM.EventsNum++;
		g_JSM.Events[i].JoyId = id;
		g_JSM.Events[i].InputId = input;
		g_JSM.Events[i].EventId = event;
		g_JSM.Events[i].Callback = cb;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Get current joystick status
// Input  : JSM_JOYSTICK
// Output : 
inline u8 JSM_GetStatus(u8 joy)
{
	return g_JSM.Process[joy].CurrentStatus;
}

//-----------------------------------------------------------------------------
// Get current direction of the given joystick
inline i8 JSM_GetStickDirection(u8 joy)
{
	u8 in = g_JSM.Process[joy].CurrentStatus;
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
	return g_JSM.Process[joy].State[in];
}

//-----------------------------------------------------------------------------
// Get current joystick state timer
inline u8 JSM_GetInputTimer(u8 joy, u8 in)
{
	return g_JSM.Process[joy].Timer[in];
}

//-----------------------------------------------------------------------------
//
const c8* JSM_GetEventName(u8 ev)
{
	switch(ev)
	{
	case JSM_EVENT_CLICK:
		return "Clck";
	case JSM_EVENT_RELEASE:
		return "Rel.";
	case JSM_EVENT_HOLD:
		return "Hold";
	case JSM_EVENT_DOUBLE_CLICK:
		return "DClk";
	case JSM_EVENT_DOUBLE_CLICK_HOLD:
		return "DCHo";
	default:
		break;
	}
	return "???";
}

#endif // USE_JOYSTICK_MANAGER
