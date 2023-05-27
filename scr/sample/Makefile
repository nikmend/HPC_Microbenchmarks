GCC = gcc
pT = -lpthread -O3
oT = -fopenmp -O3

CFLAGS = -IAPI/include -IAPI/bin -lpapi
CLinks = -m64 

BINDIR = ../BIN/

all: Opapi Otime Otime-trace OtimePar OtimePar-trace

clean:
	rm *.o 

Opapi:
	$(GCC) $(oT) -c $@.c $(CFLAGS)

Otime:
	$(GCC) $(oT) -c $@.c $(CLinks)

OtimePar:
	$(GCC) $(oT) -c $@.c $(CLinks)

Otime-trace:
	$(GCC) $(oT) -c $@.c $(CLinks)

OtimePar-trace:
	$(GCC) $(oT) -c $@.c $(CLinks)
