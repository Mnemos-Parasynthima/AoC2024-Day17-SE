CC = gcc
CC_FLAGS = -Wall
INCLUDES = -I. -Iheaders

SRCS = hardware.c machine.c main.c mem.c ./pipe/Decode.c \
				./pipe/Execute.c ./pipe/Fetch.c ./pipe/instr.c \
				./pipe/Memory.c ./pipe/Writeback.c ./pipe/hazard.c ./pipe/forward.c

HEADERS = headers/hardware.h headers/instr.h headers/machine.h \
					headers/mem.h headers/pipeline.h headers/hazard.h headers/forward.h

OBJS = $(SRCS:.c=.o)



all: se

se:  $(OBJS)
	$(CC) $(CC_FLAGS) -o se $(OBJS) -lm

%.o: %.c $(HEADERS)
	$(CC) $(CC_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) se

.PHONY: all se clean