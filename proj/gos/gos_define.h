//_____________________________________________________________________________
//   ▄▄▄        ▄▄              ▄▄     ▄▄▄                                     
//  ██   ▄█▀▄  ▄██  ██▀   ▄█▀▄ ██ ▀   ▀█▄  ▄█▀▄ ▄█▀▀ ▄█▀▀ ▄███ ██▄▀            
//  ▀█▄█ ▀█▄▀ ▀▄██ ▄██    ▀█▄▀ ██▀    ▄▄█▀ ▀█▄▀ ▀█▄▄ ▀█▄▄ ▀█▄▄ ██              
//_____________________________________________________________________________

/// Sprite ID
enum SPRITE_ID
{
	SPRITE_Score   = 0,	// 0 - 2 "X-X"
	SPRITE_Timer   = 3,	// 2 - 7 "XX:XX" 
	SPRITE_08      = 8,
	SPRITE_09      = 9,	
	SPRITE_Player1 = 10,
	SPRITE_Player2 = 11,
	SPRITE_Player3 = 12,
	SPRITE_Player4 = 13,
	SPRITE_14      = 14,
	SPRITE_15      = 15,	
	SPRITE_GoalH   = 16,	// 16 - 23
	SPRITE_GoalL   = 24,	// 24 - 26
	SPRITE_27      = 27,
	SPRITE_GoalR   = 28,	// 28 - 30
	SPRITE_31      = 31,
};

/// Player Role
enum PLAYER_ROLE
{
	ROLE_GK = 0,	///< Goal Keeper
	ROLE_DL = 1,	///< Left Defender
	ROLE_DC = 2,	///< Central Defender
	ROLE_DR = 3,	///< Right Defender
	ROLE_ML = 4,	///< Left Midfielder
	ROLE_MR = 5,	///< Right Midfielder
	ROLE_ST = 6,	///< Striker
	ROLE_MAX = 7,
};

/// Controller Type
enum CONTROLLER_TYPE
{
	CONTROL_AI,		///< Actor is controlled by artifical intelligence
	CONTROL_Input,	///< Actor is controlled by player inputs
	CONTROL_Puppet,	///< Actor is controller by specific code (wait for action)
};

/// Controller Input
enum CONTROLLER_INPUT
{
	INPUT_Joystick1,
	INPUT_Joystick2,
	INPUT_Keyboard1,
	INPUT_Keyboard2,
	INPUT_MAX = 3,
};

/// Player actions
enum PLAYER_ACTION
{
	ACTION_Idle = 0,
	ACTION_Run,
	ACTION_Charge,
	ACTION_Shoot,
	ACTION_Head,
	ACTION_Tackle,
	ACTION_Jump,
	ACTION_MAX = 15,
};

/// AI behaviors
enum AI_BEHAVIOR
{
	AI_Support = 0,
	AI_Defend,
	AI_Intercept,
	AI_MAX = 15,
};
