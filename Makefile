# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),3DS)
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif

    ifeq ($(strip $(DEVKITARM)),)
        $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
    endif
endif

# COMMON CONFIGURATION #

NAME := CTRM

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source

EXTRA_OUTPUT_FILES := servefiles

LIBRARY_DIRS := $(DEVKITPRO)/libctru
LIBRARIES := citro3d ctru m

BUILD_FLAGS :=
RUN_FLAGS :=

VERSION_MAJOR := 0
VERSION_MINOR := 1
VERSION_MICRO := 0

# 3DS CONFIGURATION #

TITLE := $(NAME)
DESCRIPTION := Open source file manager.
AUTHOR := leo60228
PRODUCT_CODE := CTR-P-CTRM
UNIQUE_ID := 0xF8056

SYSTEM_MODE := 64MB
SYSTEM_MODE_EXT := Legacy

ICON_FLAGS := --flags visible,ratingrequired,recordusage --cero 153 --esrb 153 --usk 153 --pegigen 153 --pegiptr 153 --pegibbfc 153 --cob 153 --grb 153 --cgsrr 153

ROMFS_DIR := romfs
BANNER_AUDIO := meta/audio.wav
BANNER_IMAGE := meta/banner.png
ICON := meta/icon.png

# INTERNAL #

include buildtools/make_base
