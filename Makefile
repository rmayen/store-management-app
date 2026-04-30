CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -O2
LDLIBS ?= -lssl -lcrypto
TARGET ?= StoreApp
SRC := StoreApp.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe userdata.txt log.txt error_log.txt

.PHONY: run clean
