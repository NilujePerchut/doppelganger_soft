SDCCDIR		= ../toolchain/bin
COMPILER	= SDCC
CPU			= 18f45k50
APPSTART	= 0xC00

ARCH	= pic16
OPTIMIZ	= --optimize-df --optimize-cmp --obanksel=9 --denable-peeps

# C files
SRCS		= $(wildcard src/*.c)

# SDCC compiler
CC		= $(SDCCDIR)/sdcc
OBJS	= $(SRCS:src/%.c=obj/%.o)

PFLAGS	= -V -m$(ARCH) -p$(CPU) \
		  --use-non-free \
		  --fomit-frame-pointer \
		  --std-c99

# Compiler flags
CFLAGS		= $(PFLAGS) $(OPTIMIZ) -DCHIP=$(CPU) --ivt-loc=$(APPSTART)

# Assembler flags
# -w[0|1|2] : set message level
AFLAGS		= -Wa,"-w0"

# Linker flags
# -m : output a map file
# -w : disable "processor mismatch" warning
# -s : we use our own linker script and startup code (work)
# -u : add macro value for script.
# --no-crt : do not link the default run-time modules
LDFLAGS		= -Wl -slkr/18f45k50_g.lkr \
			  -llibc18f.lib --no-crt

########################################################################
#	RULES                                                              #
########################################################################
#                                                                      #
#	$@ 	Le nom de la cible                                             #
#	$< 	Le nom de la première dépendance                               #
#	$^ 	La liste des dépendances                                       #
#	$? 	La liste des dépendances plus récentes que la cible            #
#	$* 	Le nom du fichier sans suffixe                                 #
#                                                                      #
########################################################################

all: titre Doppelganger.hex

titre:
	@echo "***********************************************"
	@echo "\033[1mDoppelganger\033[0m"
	@echo "***********************************************"

clean:
	rm -f hex/*
	rm -f obj/*

# Compile and assemble all the src/.c in obj/.o
obj/%.o: src/%.c
	@$(CC) $(CFLAGS) $(AFLAGS) -c $< -o$@

# Links the modules all together
Doppelganger.hex: $(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) -ohex/$@ $^
