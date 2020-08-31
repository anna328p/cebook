# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        ?= DEMO
COMPRESSED  ?= NO
ICON        ?= icon.png
DESCRIPTION ?= "CE C SDK Demo"

# ----------------------------

include $(CEDEV)/include/.makefile
