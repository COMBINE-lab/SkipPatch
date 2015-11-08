CC	= g++ 
CFLAGS = -c -Wall -Werror -fmax-errors=3 -g -Wunused -Wunreachable-code
LIBARG	= -O3 -funroll-loops -g -std=c++11 -pthread 
#LIBARG	= -g -std=c++11 -pthread #compiling without optimizationa for debugging
#LD_FLAGS = -lpthread
INCLUDE = -I . -I include
TARGET	= main genome utils benchmark ./skip_list/skip_list ./skip_list/skip_list_test test test_skippatch 
SRC	= xxhash.c $(addsuffix .cpp, $(TARGET))

#all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(LIBARG) $(SRC) $(INCLUDE) -o $@

clean:
	rm -f $(TARGET)


