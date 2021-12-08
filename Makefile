OUT = sim
CC = g++
LINKER = g++
DEBUGDIR = ./debug
SOURCEDIR = ./src
BUILDDIR = ./build
CFLAGS = -std=c++17 -Wall -Werror -Wunused-function -Wreturn-type -Wunused-variable -Isimlib/src -Iinclude -lsimlib -L.
DEBUGCFLAGS = -std=c++17 -g -Wall -Werror -Wunused-function -Wreturn-type -Wunused-variable -DDEBUG=1 -Isimlib/src -Iinclude -lsimlib -L.
LFLAGS = -lsimlib -L. '-Wl,-R$$ORIGIN'

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard,$(SOURCEDIR),*.cpp)
OBJECTS := $(SOURCES:$(SOURCEDIR)/%.cpp=$(BUILDDIR)/%.o)
DEBUGOBJECTS := $(SOURCES:$(SOURCEDIR)/%.cpp=$(DEBUGDIR)/%.o)

all: $(OUT)
debug: $(DEBUGDIR)/$(OUT)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	@-mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(DEBUGOBJECTS): $(DEBUGDIR)/%.o : $(SOURCEDIR)/%.cpp
	@-mkdir -p `dirname $@`
	$(CC) $(DEBUGCFLAGS) -c $< -o $@
	@echo "Compiled "$<" for debug successfully!"

$(OUT): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(DEBUGDIR)/$(OUT): $(DEBUGOBJECTS)
	@$(LINKER) $(DEBUGOBJECTS) $(LFLAGS) -o $@
	@echo "Debug linking complete!"

clean:
	rm -f $(OBJECTS) $(DEBUGOBJECTS) $(OUT) $(DEBUGDIR)/$(OUT)

.PHONY: all clear debug
