//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// Real-time clock module (RP-5C01)
//
// Réferences:
//  - Ricoh RP/RF5C01A application manual
//  - https://www.msx.org/wiki/Ricoh_RP-5C01

#pragma once

#define RTC_PORT_ADDR	0xB4
#define RTC_PORT_DATA	0xB5

__sfr __at(RTC_PORT_ADDR) g_RTC_AddrPort;
__sfr __at(RTC_PORT_DATA) g_RTC_DataPort;

