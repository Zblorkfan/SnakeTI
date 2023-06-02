# Exported from https://tiplanet.org/pb/ on Fri Jun  2 12:04:24 2023 (CEST)

# ----------------------------
# Program Options
# ----------------------------

NAME         ?= SNAKE
ICON         ?= icon.png
DESCRIPTION  ?= ""
COMPRESSED   ?= YES
ARCHIVED     ?= YES

# ----------------------------
# Compile Options
# ----------------------------

CFLAGS   ?= -Oz -W -Wall -Wextra -Wwrite-strings
CXXFLAGS ?= -Oz -W -Wall -Wextra -Wwrite-strings

# ----------------------------

include $(shell cedev-config --makefile)
