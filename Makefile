TARGET	= main
SRCS		= main.c avltree.c ftree.c zelkova.c SFMT.c
OBJS		= $(SRCS:%.c=%.o)
CC			= gcc
CFLAGS		= -W -Wall -O3 -DSFMT_MEXP=19937

# ターゲットファイル生成
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ 

# オブジェクトファイル生成
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^

# "make all" で make clean と make を実行
all: clean $(OBJS) $(TARGET)

# .oファイル，実行ファイルを削除
clean:
	rm -f $(OBJS) $(TARGET)