CC	= g++ 
CFLAGS = -c -Wall -Werror -fmax-errors=3
LIBARG	= -g -std=c++11 
INCLUDE = -I . 
TARGET	= main genome utils benchmark test
SRC	= $(addsuffix .cpp, $(TARGET))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(LIBARG) $(SRC) $(INCLUDE) -o $@

clean:
	rm -f $(TARGET)


