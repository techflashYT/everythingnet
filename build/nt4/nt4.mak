# EverythingNet - Alternative Build Tooling - NMAKE Makefile for VC++ 4.x for Microsoft Windows NT 4.0
# Copyright (C) 2025 Techflash

# Compiler and linker
CC = cl
LD = link
 
# Number of directories to move back up to
# get back to the root
RTLVL = ..\..

# Set up include dirs
INCLUDES = /I$(RTLVL)\include /I$(RTLVL)\plat\nt4\libc\include

# Preprocessor definitions
DEFINES = /DCONFIG_NET_MAX_PKT_KB=64

# Compiler flags
CFLAGS = /nologo /W3 /O2 /Zi $(INCLUDES) $(DEFINES)

# Linker flags
LDFLAGS = /nologo /debug

# Target binary
TARGET = $(RTLVL)\bin\everthingnet.exe

# Source files
SRC = $(RTLVL)\app\*.c $(RTLVL)\plat\nt4\*.c

# Object files
OBJS = $(SRC:.c=.o)

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) /OUT:$@ $(OBJS)

# Compile
#$(RTLVL)\app\%.o: %.c
.c.o:
	$(CC) $(CFLAGS) /c $<

clean:
	-del $(OBJS) $(TARGET)
