CC	= g++
CFLAGS = -c -Wall
LIBARG	= -g -std=c++11 
#INC = -I $()
TARGET	= dynamic_string
SRC	= $(addsuffix .cpp, $(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(LIBARG) -o $@

clean:
	rm -f $(TARGET)
