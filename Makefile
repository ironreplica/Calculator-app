# macOS build. Run `make` to build build/Calculator.app, `make run` to launch it.
# (Windows builds still use "Calculator app.sln" in Visual Studio.)

CXX      = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -fobjc-arc
LDFLAGS  = -framework Cocoa

APP     = build/Calculator.app
BIN     = $(APP)/Contents/MacOS/Calculator
SOURCES = mac/main.mm ExpressionTree.cpp

all: $(APP)

$(APP): $(BIN) mac/Info.plist
	cp mac/Info.plist $(APP)/Contents/Info.plist

$(BIN): $(SOURCES) ExpressionTree.h
	mkdir -p $(APP)/Contents/MacOS
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

run: all
	open $(APP)

clean:
	rm -rf build

.PHONY: all run clean
