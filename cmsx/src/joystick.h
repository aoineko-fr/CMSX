//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Joystick handler using direct access to PSG port
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Direct access functions
//-----------------------------------------------------------------------------

#define JOY_PORT_1					0
#define JOY_PORT_2					(1 << 6)

#define JOY_INPUT_DIR_NONE			0
#define JOY_INPUT_DIR_UP			(1 << 0)
#define JOY_INPUT_DIR_DOWN			(1 << 1)
#define JOY_INPUT_DIR_LEFT			(1 << 2)
#define JOY_INPUT_DIR_RIGHT			(1 << 3)
#define JOY_INPUT_DIR_UP_RIGTH		(JOY_INPUT_DIR_UP + JOY_INPUT_DIR_RIGHT)
#define JOY_INPUT_DIR_UP_LEFT		(JOY_INPUT_DIR_UP + JOY_INPUT_DIR_LEFT)
#define JOY_INPUT_DIR_DOWN_RIGTH	(JOY_INPUT_DIR_DOWN + JOY_INPUT_DIR_RIGHT)
#define JOY_INPUT_DIR_DOWN_LEFT		(JOY_INPUT_DIR_DOWN + JOY_INPUT_DIR_LEFT)
#define JOY_INPUT_DIR_MASK			0x0F
#define JOY_INPUT_TRIGGER_A			(1 << 4)
#define JOY_INPUT_TRIGGER_B			(1 << 5)

// Get the current joystick information (bit=0: pressed)
// Input  : JOY_PORT_1 or JOY_PORT_2
// Output : xxBARLDU
//            │││││└─ Up
//            ││││└── Down
//            │││└─── Left
//            ││└──── Right
//            │└───── Trigger A
//            └────── Trigger B
u8 Joystick_Read(u8 port) __FASTCALL;

// Get current direction of the given joystick
// Input  : JOY_PORT_1 or JOY_PORT_2
inline u8 Joystick_GetDirection(u8 port);

// Get current trigger status of the given joystick (0: released; 1: pressed)
// Input  : JOY_PORT_1 or JOY_PORT_2
//          JOY_INPUT_TRIGGER_A or JOY_INPUT_TRIGGER_B
inline u8 Joystick_GetTrigger(u8 port, u8 trigger);


//-----------------------------------------------------------------------------
// Advanced joystick manager
//-----------------------------------------------------------------------------
#if USE_JOYSTICK_MANAGER

enum JSM_JOYSTICK
{
	JSM_JOYSTICK_1,
	JSM_JOYSTICK_2,
	//---------------------------------
	JSM_JOYSTICK_MAX,
	JSM_JOYSTICK_ANY,
};

enum JSM_INPUT
{
	JSM_INPUT_STICK = 0,
	JSM_INPUT_BUTTON_A,
	JSM_INPUT_BUTTON_B,
	//---------------------------------
	JSM_INPUT_MAX,
	JSM_INPUT_ANY,
};

#define JSM_STATE_OFF			0x00
#define JSM_STATE_PRESS			0x01
#define JSM_STATE_ON			0x02
#define JSM_STATE_RELEASE		0x03
#define JSM_STATE_PRESSMASK		0x03

#define JSM_STATE_HOLD			0x10
#define JSM_STATE_HOLDING		0x20
#define JSM_STATE_HOLDMASK		0x30

#define JSM_STATE_DOUBLE		0x80


enum JSM_EVENT
{
	JSM_EVENT_CLICK = 0,
	JSM_EVENT_HOLD,
	JSM_EVENT_DOUBLE_CLICK,	
	JSM_EVENT_DOUBLE_CLICK_HOLD,
	JSM_EVENT_RELEASE,
	//---------------------------------
	JSM_EVENT_MAX,
	JSM_EVENT_ANY,
	JSM_EVENT_NONE,
};

#define JSM_EVENT_TAB_SIZE	16

typedef void (*jsm_cb)(u8 joy, u8 in, u8 evt);
typedef u8 (*jsm_check)(u8 joy, u8 in);

typedef struct
{
	u8			JoyId;
	u8			InputId;
	u8			EventId;
	jsm_cb		Callback;
} JSM_Event;

typedef struct
{
	u8			CurrentStatus;
	u8			PreviousStatus;
	u8			State[JSM_INPUT_MAX];
	u8			Timer[JSM_INPUT_MAX];
} JSM_Process;	

typedef struct
{
	u8			HoldTimer;
	u8			LongClickTimer;
	u8			DoubleClickTimer;
	u8			DiscardTimer;
} JSM_Config;	

typedef struct
{
	JSM_Process	Process[JSM_JOYSTICK_MAX];
	JSM_Event	Events[JSM_EVENT_TAB_SIZE];
	i8			EventsNum;
	jsm_check	Checker[JSM_EVENT_MAX];
	JSM_Config  Config;
} JSM_Data;

extern JSM_Data g_JSM;

// Initialize Joystick manager
void JSM_Initialize();

// Update Joystick manager
void JSM_Update();

// Register a callback to a given Joystick manager's event
bool JSM_RegisterEvent(u8 joy, u8 input, u8 event, jsm_cb cb);

// Get current joystick status
inline u8 JSM_GetStatus(u8 joy);

// Get current direction of the given joystick
inline i8 JSM_GetStickDirection(u8 joy);

// Get current joystick state
inline u8 JSM_GetInputState(u8 joy, u8 in);

// Get current joystick state timer
inline u8 JSM_GetInputTimer(u8 joy, u8 in);

//
const c8* JSM_GetEventName(u8 ev);

#endif // USE_JOYSTICK_MANAGER






















