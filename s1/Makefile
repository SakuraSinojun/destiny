

CXX	:= g++
LD	:= g++
AR	:= ar
CXXFLAGS := -Wall -O2 -Werror
INCLUDES = -Ilog -I.

TARGET	= a.out
READLOG = r.out

LINKS	= -L.
LIBS	= -lncursesw

SOURCES := main.cc
SOURCES += game.cc
SOURCES += ui.cc
SOURCES += gamemap.cc
SOURCES += race.cc
SOURCES += creature.cc
SOURCES += hero.cc
SOURCES += attr.cc
SOURCES += class.cc
SOURCES += dice.cc
SOURCES += scene.cc
SOURCES += scene_genhero.cc
SOURCES += scene_game.cc
SOURCES += monster.cc
SOURCES += color.cc
SOURCES += item.cc
SOURCES += rule.cc

SOURCES += log/logging.cc
SOURCES += log/log_dispatcher.cc
SOURCES += log/logtracer_shell.cc
SOURCES += log/logtracer_udp.cc

SOURCES += tools/dump.cc
SOURCES += tools/config.cc
SOURCES += tools/string_util.cc
SOURCES += tools/socket.cc


OBJS := $(SOURCES:.cc=.o)
DEPS := $(SOURCES:.cc=.d)

all: prebuild $(TARGET) $(READLOG)

$(TARGET): $(OBJS)
	@echo Linking $@ ...
	$(LD) $(OBJS) $(LINKS) $(LIBS) -o$@
	@echo -------------------------------------------
	@echo done.

$(READLOG): readlog.o tools/socket.o
	@echo Linking $@ ...
	$(LD) readlog.o tools/socket.o  -o$@
	@echo -------------------------------------------
	@echo done.

.cpp.o:
	@echo Compling $@ ...
	$(CXX) -c $< $(INCLUDES) $(CXXFLAGS)  -o $@
	@echo -------------------------------------------

.cc.o:
	@echo Compling $@ ...
	$(CXX) -c $< $(INCLUDES) $(CXXFLAGS)  -o $@
	@echo ------------------------------------------

%.d:%.cc
	$(CXX) -MM $< $(INCLUDES) $(CXXFLAGS) -o $@

prebuild:
	#echo -------

include $(DEPS)

clean:
	rm -fr $(OBJS) $(DEPS) readlog.o wincurse.o



