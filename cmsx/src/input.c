//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// User input handler using direct access to ports
//-----------------------------------------------------------------------------
#include "core.h"
#include "ports.h"
#include "input.h"
#include "memory.h"

//-----------------------------------------------------------------------------
//
// Direct access to joystick
//
//-----------------------------------------------------------------------------
#if (USE_INPUT_JOYSTICK || USE_INPUT_MANAGER)

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
u8 Joystick_GetDirection(u8 port) __FASTCALL
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
/*inline u8 Joystick_GetTrigger(u8 port, u8 trigger)
{
	u8 in = Joystick_Read(port);
	return ((in & trigger) == 0);
}*/

#endif // (USE_INPUT_JOYSTICK || USE_INPUT_MANAGER)


//-----------------------------------------------------------------------------
//
// Direct access to keyboard
//
//-----------------------------------------------------------------------------
#if (USE_INPUT_KEYBOARD || USE_INPUT_MANAGER)

//-----------------------------------------------------------------------------
// Read keyboard matrix row
u8 Keyboard_Read(u8 row) __FASTCALL
{
	row;
	// FastCall
	//	ld		l, key
	__asm
		in		a, (P_PPI_C)
		and		#0xF0			// only change bits 0-3
		or		l				// take row number from L
		out		(P_PPI_C), a
		in		a, (P_PPI_B)	// read row into A
		ld		l, a
	__endasm;
}

//-----------------------------------------------------------------------------
// Check if a given key is pressed
u8 Keyboard_IsKeyPressed(u8 key) __FASTCALL
{
	return (Keyboard_Read(key & 0x0F) & (1 << (key >> 4))) == 0;
}

#endif // (USE_INPUT_KEYBOARD || USE_INPUT_MANAGER)


//-----------------------------------------------------------------------------
//
// Complete input manager
//
//-----------------------------------------------------------------------------
#if USE_INPUT_MANAGER

IPM_Data g_IPM;

//-----------------------------------------------------------------------------
// Check if a click event occured
u8 CheckClick(u8 dev, u8 input)
{
	return (g_IPM.Process[dev].State[input] == IPM_STATE_PRESS);
}

//-----------------------------------------------------------------------------
// Check if a release event occured
u8 CheckRelease(u8 dev, u8 input)
{
	return (g_IPM.Process[dev].State[input] == IPM_STATE_RELEASE);
}

//-----------------------------------------------------------------------------
// Check if a hold event occured
u8 CheckHold(u8 dev, u8 input)
{
	return (g_IPM.Process[dev].State[input] == (IPM_STATE_ON + IPM_STATE_HOLD));
}

//-----------------------------------------------------------------------------
// Check if a double-click event occured
u8 CheckDoubleClick(u8 dev, u8 input)
{
	return (g_IPM.Process[dev].State[input] == (IPM_STATE_PRESS + IPM_STATE_DOUBLE));
}

//-----------------------------------------------------------------------------
// Check if a double-click-hold event occured
u8 CheckDoubleClickHold(u8 dev, u8 input)
{
	return (g_IPM.Process[dev].State[input] == (IPM_STATE_ON + IPM_STATE_DOUBLE + IPM_STATE_HOLD));
}

//-----------------------------------------------------------------------------
// Initialize input manager
// Inupts:		config		The manager configuration (null: default config will be used)
void IPM_Initialize(IPM_Config* config)
{
	for(u8 dev = 0; dev < IPM_DEVICE_MAX; ++dev)
	{
		g_IPM.Process[dev].CurrentStatus  = 0xFF;
		g_IPM.Process[dev].PreviousStatus = 0xFF;
		
		for(u8 in = 0; in < IPM_INPUT_MAX; ++in)
		{
			g_IPM.Process[dev].State[in] = 0;
			g_IPM.Process[dev].Timer[in] = 0xFF;
		}
	}
	
	g_IPM.EventsNum = 0;
	g_IPM.Checker[IPM_EVENT_CLICK]             = CheckClick;
	g_IPM.Checker[IPM_EVENT_RELEASE]           = CheckRelease;
	g_IPM.Checker[IPM_EVENT_HOLD]              = CheckHold;
	g_IPM.Checker[IPM_EVENT_DOUBLE_CLICK]      = CheckDoubleClick;
	g_IPM.Checker[IPM_EVENT_DOUBLE_CLICK_HOLD] = CheckDoubleClickHold;
	
	if(config == null)
		config = &g_DefaultConfig;
		
	Mem_Copy(config, &g_IPM.Config, sizeof(g_IPM.Config));
}

//-----------------------------------------------------------------------------
// Update input manager
void IPM_Update()
{
	// Update device status
	for(u8 dev = 0; dev < IPM_DEVICE_MAX; ++dev)
	{
		IPM_Process* proc = &g_IPM.Process[dev];
		proc->PreviousStatus = proc->CurrentStatus;
		
		if(dev <= IPM_DEVICE_JOYSTICK_2) // Get joystick current status
		{
			proc->CurrentStatus = Joystick_Read((dev == IPM_DEVICE_JOYSTICK_1) ? JOY_PORT_1 : JOY_PORT_2);
		}
		else // Get keyboard-set current status
		{
			u8 ks = dev - IPM_DEVICE_KEYBOARD_1;
			u8 flag = 0xFF;

			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].Up))
				flag &= ~JOY_INPUT_DIR_UP;
			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].Right))
				flag &= ~JOY_INPUT_DIR_RIGHT;
			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].Down))
				flag &= ~JOY_INPUT_DIR_DOWN;
			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].Left))
				flag &= ~JOY_INPUT_DIR_LEFT;
			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].TriggerA))
				flag &= ~JOY_INPUT_TRIGGER_A;
			if(Keyboard_IsKeyPressed(g_IPM.Config.KeySet[ks].TriggerB))
				flag &= ~JOY_INPUT_TRIGGER_B;

			proc->CurrentStatus = flag;
		}

		u8 in;
		for(in = 0; in < IPM_INPUT_MAX; ++in)
		{
			// Increment previous state counter
			if(proc->Timer[in] < 0xFF)
				proc->Timer[in]++;
			
			// Check state tranition
			if((proc->State[in] & IPM_STATE_PRESSMASK) == IPM_STATE_PRESS) // PRESS >> ON
			{
				proc->State[in] &= ~IPM_STATE_PRESSMASK;
				proc->State[in] |= IPM_STATE_ON;
			}			
			else if((proc->State[in] & IPM_STATE_PRESSMASK) == IPM_STATE_RELEASE) // RELEASE >> OFF
			{
				proc->State[in] &= ~IPM_STATE_PRESSMASK;
				proc->State[in] &= ~IPM_STATE_HOLDMASK;
				proc->State[in] |= IPM_STATE_OFF;
			}
			else if(((proc->State[in] & IPM_STATE_HOLDMASK) == 0) && ((proc->State[in] & IPM_STATE_PRESSMASK) == IPM_STATE_ON) && (proc->Timer[in] > g_IPM.Config.HoldTimer)) // ON >> ON + HOLD
			{
				proc->State[in] |= IPM_STATE_HOLD;
			}
			else if(((proc->State[in] & IPM_STATE_HOLDMASK) == IPM_STATE_HOLD)) // ON + HOLD >> ON + HOLDING
			{
				proc->State[in] &= ~IPM_STATE_HOLDMASK;
				proc->State[in] |= IPM_STATE_HOLDING;
			}
			else if(((proc->State[in] & IPM_STATE_PRESSMASK) == IPM_STATE_OFF) && (proc->Timer[in] > g_IPM.Config.DoubleClickTimer)) // OFF + DBL >> OFF
			{
				proc->State[in] &= ~IPM_STATE_DOUBLE;
			}

			u8 curOn;
			u8 prevOn;			
			if(in == IPM_INPUT_STICK)
			{
				curOn = (proc->CurrentStatus & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK;
				prevOn = (proc->PreviousStatus & JOY_INPUT_DIR_MASK) != JOY_INPUT_DIR_MASK;
			}
			else
			{
				u8 mask = (in == IPM_INPUT_BUTTON_A) ? JOY_INPUT_TRIGGER_A : JOY_INPUT_TRIGGER_B;
				curOn = (proc->CurrentStatus & mask) == 0;
				prevOn = (proc->PreviousStatus & mask) == 0;
			}

			if(curOn && !prevOn) // OFF >> PRESS
			{
				proc->State[in] &= ~IPM_STATE_PRESSMASK;
				proc->State[in] |= IPM_STATE_PRESS;
				if(proc->Timer[in] <= g_IPM.Config.DoubleClickTimer)
					proc->State[in] |= IPM_STATE_DOUBLE;
				proc->Timer[in] = 0;
			}
			else if(!curOn && prevOn) // ON >> RELEASE
			{
				proc->State[in] = IPM_STATE_RELEASE;
				proc->Timer[in] = 0;
			}
		}
	}
	
	// Check registered events
	for(u8 i = 0; i < g_IPM.EventsNum; ++i)
	{
		IPM_Event* entry = &g_IPM.Events[i];
		
		IPM_cb cb = entry->Callback;
		if(cb == null)
			continue;

		if(entry->Device < IPM_DEVICE_MAX)
		{	
			u8 dev = entry->Device;
			if(entry->Event < IPM_EVENT_MAX)
			{
				u8 ev = entry->Event;
				if(entry->Input < IPM_INPUT_MAX)
				{
					u8 in = entry->Input;
					if(g_IPM.Checker[ev](dev, in))
						cb(dev, in, ev);
				}
				else // IPM_INPUT_ANY
				{
					for(u8 in = 0; in < IPM_INPUT_MAX; ++in)
						if(g_IPM.Checker[ev](dev, in))
							cb(dev, in, ev);			
				}
			}
			else // IPM_EVENT_ANY
			{
				for(u8 ev = 0; ev < IPM_EVENT_MAX; ++ev)
				{
					if(entry->Input < IPM_INPUT_MAX)
					{
						u8 in = entry->Input;
						if(g_IPM.Checker[ev](dev, in))
							cb(dev, in, ev);
					}
					else // IPM_INPUT_ANY
					{
						for(u8 in = 0; in < IPM_INPUT_MAX; ++in)
							if(g_IPM.Checker[ev](dev, in))
								cb(dev, in, ev);			
					}
				}
			}
		}	
		else // IPM_DEVICE_ANY
		{	
			for(u8 dev = 0; dev < IPM_DEVICE_MAX; ++dev)
			{
				if(entry->Event < IPM_EVENT_MAX)
				{
					u8 ev = entry->Event;
					if(entry->Input < IPM_INPUT_MAX)
					{
						u8 in = entry->Input;
						if(g_IPM.Checker[ev](dev, in))
							cb(dev, in, ev);
					}
					else // IPM_INPUT_ANY
					{
						for(u8 in = 0; in < IPM_INPUT_MAX; ++in)
							if(g_IPM.Checker[ev](dev, in))
								cb(dev, in, ev);			
					}
				}
				else // IPM_EVENT_ANY
				{
					for(u8 ev = 0; ev < IPM_EVENT_MAX; ++ev)
					{
						if(entry->Input < IPM_INPUT_MAX)
						{
							u8 in = entry->Input;
							if(g_IPM.Checker[ev](dev, in))
								cb(dev, in, ev);
						}
						else // IPM_INPUT_ANY
						{
							for(u8 in = 0; in < IPM_INPUT_MAX; ++in)
								if(g_IPM.Checker[ev](dev, in))
									cb(dev, in, ev);			
						}
					}
				}
			}
		}	
	}
}

//-----------------------------------------------------------------------------
// Register a callback to a given device manager's event
bool IPM_RegisterEvent(u8 dev, u8 input, u8 event, IPM_cb cb)
{
	if(g_IPM.EventsNum < IPM_EVENT_TAB_SIZE - 1)
	{
		u8 i = g_IPM.EventsNum++;
		g_IPM.Events[i].Device = dev;
		g_IPM.Events[i].Input = input;
		g_IPM.Events[i].Event = event;
		g_IPM.Events[i].Callback = cb;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Get current device status
// Input  : IPM_DEVICE
// Output : 
inline u8 IPM_GetStatus(u8 dev)
{
	return g_IPM.Process[dev].CurrentStatus;
}

//-----------------------------------------------------------------------------
// Get current direction of the given device
inline u8 IPM_GetStickDirection(u8 dev)
{
	u8 in = g_IPM.Process[dev].CurrentStatus;
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
	
	return 0xFF;
}

//-----------------------------------------------------------------------------
// Get current device state
inline u8 IPM_GetInputState(u8 dev, u8 input)
{
	return g_IPM.Process[dev].State[input];
}

//-----------------------------------------------------------------------------
// Get current device state timer
inline u8 IPM_GetInputTimer(u8 dev, u8 input)
{
	return g_IPM.Process[dev].Timer[input];
}

//-----------------------------------------------------------------------------
//
const c8* IPM_GetEventName(u8 ev)
{
	switch(ev)
	{
	case IPM_EVENT_CLICK:
		return "Clck";
	case IPM_EVENT_RELEASE:
		return "Rel.";
	case IPM_EVENT_HOLD:
		return "Hold";
	case IPM_EVENT_DOUBLE_CLICK:
		return "DClk";
	case IPM_EVENT_DOUBLE_CLICK_HOLD:
		return "DCHo";
	default:
		break;
	}
	return "???";
}

#endif // USE_INPUT_MANAGER