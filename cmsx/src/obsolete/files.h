//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

// File I/O for MSX-DOS

/* standard descriptors */
#define  STDIN   0
#define  STDOUT  1
#define  STDERR  2
#define  AUX     3
#define  PRN     4

/* open/creat flags */
#define  O_RDONLY   0x01
#define  O_WRONLY   0x02
#define  O_RDWR     0x00
#define  O_INHERIT  0x04

/* creat attributes */
#define  ATTR_RDONLY  0x01
#define  ATTR_HIDDEN  0x02
#define  ATTR_SYSTEM  0x04
#define  ATTR_VOLUME  0x08
#define  ATTR_FOLDER  0x10
#define  ATTR_ARCHIV  0x20
#define  ATTR_DEVICE  0x80

/* seek whence */
#define  SEEK_SET  0
#define  SEEK_CUR  1
#define  SEEK_END  2

extern unsigned char last_error;

extern char open(char *, unsigned char);
extern char creat(char *, unsigned char, unsigned char);
extern char close(char);
extern char dup(char);
extern int read(char, void *, int);
extern int write(char, void *, int);
extern long lseek(char, long, char);
extern void exit(char);

