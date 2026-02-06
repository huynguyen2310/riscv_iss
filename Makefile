SYSTEMC_HOME ?= /usr/local/systemc-3.0.2

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(SYSTEMC_HOME)/include
LDFLAGS = -L$(SYSTEMC_HOME)/lib -lsystemc

TARGET = riscv_tlm
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
