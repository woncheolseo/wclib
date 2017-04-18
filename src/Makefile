.SUFFIXES = .cpp .o

SRCS = $(wildcard *.cpp util/*.cpp)
#SRCH = $(SRCS:.cpp=.h)
SRCH = *.h
OBJS = $(SRCS:.cpp=.o)
#INC = -I$(MYSQL_INC) -I$(ORACLE_INC)
DIR_LIB = ../bin/lib/
DIR_INC = ../bin/inc/

#CFLAGS = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -fPIC -O2 -std=c++11 -c -D_REENTRANT -Wno-write-strings -Wno-ignored-qualifiers
CFLAGS = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -fPIC -O2 -std=c++11 -c -D_REENTRANT
#CFLAGD = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -fPIC -O2 -std=c++11 -c -D_REENTRANT -g -D_DEBUG -pg
CFLAGD = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -fPIC -O2 -std=c++11 -c -D_REENTRANT -g -D_DEBUG

TARGETS = libso
TARGETA = liba
TARGETSO = libwc.so.0.9.0
TARGETSL = $(TARGETSO:.so.0.9.0=.so)

#공유LIB
$(TARGETS) : $(OBJS)
	$(CXX) -shared -o $(TARGETSO) $^
	ln -s $(TARGETSO) $(TARGETSL)	
	rm -rf $(DIR_LIB)$(TARGETSL)
	mv $(TARGETSO) $(DIR_LIB)
	mv $(TARGETSL) $(DIR_LIB)
#	cp -f $(TARGETSO) $(DIR_LIB)
#	ln -s $(DIR_LIB)$(TARGETSO) $(DIR_LIB)$(TARGETSL)
	cp -f $(SRCH) $(DIR_INC)

.cpp.o :
	$(CXX) $(INC) $(CFLAGS) $^ -o $@
#	$(CXX) $< $(INC) $(CFLAGS)

#정적LIB
$(TARGETA) :
	$(CXX) $(SRCS) $(INC) -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -O2 -c -g -D_DEBUG

#삭제
clean :
	find ./ -name "*.o" | xargs rm -f
	find ./ -name "*.bak" | xargs rm -f
	find ./ -name "*.log" | xargs rm -f
	find ./ -name "*.cache" | xargs rm -f
#	rm -rf *.o *.bak *.log *.cache

#삭제-라이브러리
cleanlib :
	rm -rf $(DIR_PLIB)$(TARGETSO) $(DIR_PLIB)$(TARGETSL)
	cd $(DIR_INC) && rm -f $(SRCH)

#새로
new :
	clear
	$(MAKE) clean
	$(MAKE) cleanlib
	$(MAKE) $(TARGETS)

#디버그
debug :
	$(CXX) $(SRCS) $(INC) $(CFLAGD)
	$(CXX) -pg -shared -o $(TARGETSO) $(OBJS)
	rm -rf $(DIR_PLIB)$(TARGETSL)
	mv $(TARGETSO) $(DIR_PLIB)
	ln -s $(DIR_PLIB)$(TARGETSO) $(DIR_PLIB)$(TARGETSL)
	cp $(SRCH) $(DIR_INC)
