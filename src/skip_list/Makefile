CC	= g++ 
CFLAGS = -c -Wall -Werror -fmax-errors=3
LIBARG	= -g -O3 -funroll-loops -std=c++11 
INCLUDE = -I . 
TARGET	= skip_list skip_list_test
SRC	= $(addsuffix .cpp, $(TARGET))

#all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(LIBARG) $(SRC) $(INCLUDE) -o $@

clean:
	rm -f $(TARGET)


