/* 覚書 */

# sqlite3(dev)のインストールが必要
apt-get install libsqlite3-dev
または
yum install sqlite-devel

# Makefile定義
CC	= /usr/bin/g++
CFLAGS = -g -O0 -Wall -fPIC -std=c++0x -fpermissive -D_TRACE_ -D_DEBUG_
LIBS = -lpthread -lexpat -lrt -lsqlite3

