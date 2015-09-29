CC	= g++ 
CFLAGS = -c -Wall -Werror -fmax-errors=3
LIBARG	= -O3 -funroll-loops -g -std=c++11 -pthread
#LD_FLAGS = -lpthread
INCLUDE = -I . -I include
TARGET	= main genome utils benchmark ./skip_list/skip_list ./skip_list/skip_list_test test test_prob 
SRC	= xxhash.c $(addsuffix .cpp, $(TARGET))

#all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(LIBARG) $(SRC) $(INCLUDE) -o $@

clean:
	rm -f $(TARGET)


