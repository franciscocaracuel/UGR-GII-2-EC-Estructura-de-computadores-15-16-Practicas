###############################################################################
# Makefile
###############################################################################

SHELL = /bin/bash # for ubuntu

###############################################################################

SRC = $(wildcard *.c *.cc)
EXE = $(basename $(SRC))
ATT = $(EXE:=.att)
DAT = $(EXE:=.dat)
PNG = $(DAT:.dat=.png)
SVG = $(DAT:.dat=.svg)

###############################################################################

CFLAGS = -fno-omit-frame-pointer -g -m32 -march=native -O3 -Wall
CXXFLAGS = $(CFLAGS) -std=c++11

###############################################################################

default: $(EXE) $(DAT) $(SVG)

all: default $(ATT) $(PNG)

clean:
	$(RM) -rfv $(ATT) $(DAT) $(EXE) $(PNG) $(SVG) core.* *~

###############################################################################

popcnt: popcnt.cpp
	g++ -O3 popcnt.cpp -o popcnt -mpopcnt -mssse3 #-fopenmp

###############################################################################

%.att: %
	objdump -C -d $< > $@

%.dat: %
	./$< | tee $@

#%.png: %.svg
#	convert $< $@

%.png: %.dat Makefile
	@gnuplot <<< "\
		set terminal pngcairo enhanced; \
		set output '$@'; \
		set key top center horizontal; \
		set grid; \
		set ylabel 'time (ms)'; \
		set y2label 'cpu cycles'; \
		set xtics rotate; \
		set ytics; \
		set y2tics; \
		set y2tics nomirror; \
		set style fill solid 0.10 border; \
		set boxwidth 0.80; \
		plot '$<' u 5:xtic(1) w boxes axes x1y1 title 'time', \
		     '$<' u 3:xtic(1) w lp pt 7 axes x1y2 title 'cycles'"

%.svg: %.dat Makefile
	@gnuplot <<< "\
		set terminal svg; \
		set output '$@'; \
		set key top center horizontal; \
		set grid; \
		set ylabel 'time (ms)'; \
		set y2label 'cpu cycles'; \
		set xtics rotate; \
		set ytics; \
		set y2tics; \
		set y2tics nomirror; \
		set style fill solid 0.10 border; \
		set boxwidth 0.80; \
		set logscale y; \
		set logscale y2; \
		plot '$<' u 5:xtic(1) w boxes axes x1y1 title 'time', \
		     '$<' u 3:xtic(1) w lp pt 7 axes x1y2 title 'cycles'"

###############################################################################

.PHONY: all clean default
.PRECIOUS: $(DAT)
.NOEXPORT:

###############################################################################
