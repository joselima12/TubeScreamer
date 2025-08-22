# Project Name
TARGET = main

# Sources
CPP_SOURCES = main.cpp RCFilter.cpp TubeScreamer.cpp Oversampler2x.cpp TSClipping.cpp

# Library Locations
DAISYSP_DIR ?= ../../DaisySP
LIBDAISY_DIR ?= ../../libDaisy

# ✅ Include your custom path here
# Add -Ichowdsp_wdf or -I. depending on where your chowdsp_wdf/ folder is
C_INCLUDES += -I.

CPPFLAGS += -std=gnu++17

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile