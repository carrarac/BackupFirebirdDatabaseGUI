CXX = g++
WX_FLAGS = $(shell wx-config --static --cxxflags)
WX_LIBS = $(shell wx-config --static --libs)
LD_FLAGS = -mwindows
WX_RES = resource.o

CXXFLAGS = -finput-charset=UTF-8 -fexec-charset=UTF-8

TARGET = app.exe
SRC = main.cpp

all: $(TARGET)

release: CXXFLAGS += -O2 -s -DNDEBUG
release: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(WX_RES) $(LD_FLAGS) $(CXXFLAGS) $(WX_FLAGS) $(WX_LIBS) \
	-static -static-libstdc++ -static-libgcc \
	-o $(TARGET) && ./$(TARGET)
clean:
	rm -f $(TARGET)