CXX         := g++
SRCDIR      := src
INCDIR      := include
OBJDIR      := obj
BASE_TARGET := Jungler

BASEFLAGS   := -std=gnu++20

PRODFLAGS   := -O2 -Wall -Wextra -Wshadow

DEBUGFLAGS  := -g -O0 -DDEBUG -fsanitize=address

.DEFAULT_GOAL := debug

BUILD := $(firstword $(MAKECMDGOALS))
ifeq ($(BUILD),)
  BUILD := debug
endif

ifeq ($(BUILD),prod)
  CXXFLAGS := $(BASEFLAGS) $(PRODFLAGS)
else
  CXXFLAGS := $(BASEFLAGS) $(DEBUGFLAGS)
endif

ifeq ($(RS),1)
  CXXFLAGS += -DRS
endif

ifeq ($(MCTS),1)
  CXXFLAGS += -DMCTS
endif

ifeq ($(RAND),1)
  CXXFLAGS += -DRAND
endif

ifeq ($(DENRAND),1)
  CXXFLAGS += -DDENRAND
endif

ifeq ($(QLEARN),1)
  CXXFLAGS += -DQLEARN
endif

ifeq ($(TRAIN),1)
  CXXFLAGS += -DTRAIN
endif


TARGET := $(BASE_TARGET)_$(BUILD)

MAIN_C  := $(SRCDIR)/Main.cpp
MAIN_O  := $(OBJDIR)/Main.o
SRC_CPP := $(filter-out $(MAIN_C), $(wildcard $(SRCDIR)/*.cpp))
SRC_O   := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC_CPP))

.PHONY: all debug prod compile clean time size

all: debug

debug: compile

prod: compile

# KOMPILACJA I LINKOWANIE
compile: $(MAIN_O) $(SRC_O) | $(OBJDIR) $(INCDIR)
	@echo "BUILD=$(BUILD)"
	@echo "Linking $(TARGET)"
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o $(TARGET) $(MAIN_O) $(SRC_O)


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(INCDIR)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Pomiar czasu wykonania
time: all
	@/usr/bin/time -v ./$(TARGET)

# Sprawdzenie rozmiaru i zależności
size: all
	@ldd ./$(TARGET) \
	  | awk '{print $$3}' \
	  | grep -v '(' \
	  | xargs du -ch \
	  | tail -n 1

clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJDIR) $(BASE_TARGET)_*
