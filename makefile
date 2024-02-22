CC = gcc
CFLAGS = -std=c99 -Wall -Werror -g -Wextra
EXE = ebcBlock ebcUnblock ebcR32 ebcU32 ebcR128 ebcU128

all: ${EXE}

clean:
	rm -rf *.o ${EXE}

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lm

ebcBlock: ebcBlock.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm

ebcUnblock: ebcUnblock.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm

ebcR32: ebcR32.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o ebcrUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm

ebcU32: ebcU32.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o ebcrUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm

ebcR128: ebcR128.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o ebcrUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm

ebcU128: ebcU128.o blockUtils.o ebcUtils.o ebUniversalUtils.o bitTwiddlingUtils.o ebcrUtils.o
	$(CC) $(CCFLAGS) $^ -o $@ -lm