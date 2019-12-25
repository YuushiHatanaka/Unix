#-------------------------------------------------------------------------------
# hcmsv3用メイクファイル
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# 各種定義
#-------------------------------------------------------------------------------
CC = g++
CXX = /usr/bin/g++
CHECK = /usr/bin/cppcheck

RELEASEFLAGS = -O0

CFLAGS = ${RELEASEFLAGS} -Wall -fPIC

INCLUDES = -I./include -I../common/include -I../hcmcmn/include

LDFLAGS = -Wall -L/usr/local/lib -L./lib -L../hcmcmn/lib

LIBS = -ldl -lm -lpthread -lnsl -lexpat -lssl -lcrypto

# コマンド定義
RM = /bin/rm -f
MKDIR = /bin/mkdir -p
INSTALL = /usr/bin/install -c
INSTALL_HEADER = ${INSTALL} -m 644
INSTALL_BIN = /usr/bin/install -m u+w,a+xr
LN = /bin/ln -s

# ディレクトリ定義
SRCDIR = ./src
OBJDIR = ./obj
LIBDIR = ./lib

# バージョン定義
MAJOR = 3
MINOR = 0
BUILD = 0
VERSION = $(MAJOR).$(MINOR).$(BUILD)

#-------------------------------------------------------------------------------
# ターゲット定義
#-------------------------------------------------------------------------------
TARGETDIR = /home/httpsim$(VERSION)/bin/
TARGET = common

OBJS= \
	$(OBJDIR)/common.o

#-------------------------------------------------------------------------------
# ルール定義
#-------------------------------------------------------------------------------
$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ -c $<

#-------------------------------------------------------------------------------
# ビルド
#-------------------------------------------------------------------------------
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)  -o $@

install :
	$(MKDIR) $(TARGETDIR)
	$(INSTALL_BIN) $(TARGET) $(TARGETDIR)$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)

remake : clean all
