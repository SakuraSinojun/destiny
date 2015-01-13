

CXX	:= g++
LD	:= g++
AR	:= ar
CXXFLAGS := -Wall -O2 -Werror
INCLUDES = -Ilog -I.

TARGET	= a.out

LINKS	= -L.
LIBS	= -lncursesw -lpthread

SOURCES := main.cpp
SOURCES += tools/logging.cpp
SOURCES += tools/dump.cpp

OBJS := $(SOURCES:.cpp=.o)
DEPS := $(SOURCES:.cpp=.d)

all: prebuild $(TARGET)

$(TARGET): $(OBJS)
	@echo Linking $@ ...
	$(LD) $(OBJS) $(LINKS) $(LIBS) -o$@
	@echo -------------------------------------------
	@echo done.

.cpp.o:
	@echo Compling $@ ...
	$(CXX) -c $< $(INCLUDES) $(CXXFLAGS)  -o $@
	@echo -------------------------------------------

%.d:%.cpp
	$(CXX) -MM $< $(INCLUDES) $(CXXFLAGS) -o $@

prebuild:
	#echo -------

include $(DEPS)

clean:
	rm -fr $(OBJS) $(DEPS) readlog.o wincurse.o



