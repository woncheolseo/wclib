.SUFFIXES = .cpp .o

SRCS = sam_log.cpp
SRCR = ../work/*.cpp
SRCD = ../work/*.cpp
SRCP = ../work/*.cpp
SRCV = ../work/*.cpp
OBJS = $(SRCS:.cpp=.o)
BACK = *.bak

HOME = /home/nwaf
TERM = xterm

#INC = -I$(MYSQL_INC) -I$(ORACLE_INC) -I$(PROJECT_HOME)/aprogram/include
#INC = -I$(MYSQL_INC) -I$(ORACLE_INC) -I$(PROJECT_HOME)/aprogram/include -I/usr/include/libxml2 -I/opt/tcmalloc/include/google -I/opt/libmemcached/include
#INC = -I/opt/libmemcached/include
INC = -I$(HOME)/aprogram/inc
INCR = -I../work
INCD = -I../work
INCP = -I../work
INCV = -I../work

#CFLAGS = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -c
CFLAGS = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -O2
#CFLAGS = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -g
CFLAGR = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -O2
#CFLAGD = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -g -D_DEBUG
CFLAGD = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -g
CFLAGP = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -O2 -pg 
CFLAGV = -W -Wall -Wno-unused-parameter -Wconversion -Wshadow -Wcast-qual -D_REENTRANT -std=c++11 -c -g -D_DEBUG

#LFLAGS = -lwc -lpthread -lrt -lmysqlclient -locci -lociei -lnnz11 -lclntsh -lgmock
#LFLAGS = -lwc -lacti -lpthread -lrt -lmysqlclient -locci -lociei -lnnz11 -lclntsh -liconv -lxml2 -ltcmalloc -lmemcached -lgmock
#LFLAGS = -lmemcached
#LFLAGS = -lpthread -lrt -lmysqlclient -locci -lociei -lnnz11 -lclntsh -lgmock -lmemcached -lwc
LFLAGS = -lwc -lpthread -lgmock -lboost_system -lpqxx -lcrypto
LFLAGR = -lpthread -lgmock -lboost_system -lpqxx
LFLAGD = -lpthread -lgmock -lboost_system -lpqxx
LFLAGP = -lpthread -lgmock -lboost_system -lpqxx-pg
LFLAGV = -lpthread -lgmock -lboost_system -lpqxx

#LPATHS  = -L$(MYSQL_LIB) -L$(ORACLE_LIB) -L$(PROJECT_HOME)/aprogram/lib
#LPATHS  = -L$(MYSQL_LIB) -L$(ORACLE_LIB) -L$(PROJECT_HOME)/aprogram/lib -L/opt/tcmalloc/lib -L/opt/libmemcached/lib
#LPATHS  = -L/opt/libmemcached/lib
LPATHS  = -L$(HOME)/aprogram/lib
LPATHR  = 
LPATHD  =
LPATHP  = 
LPATHV  = 

TARGET = $(SRCS:.cpp=)

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LPATHS) $(LFLAGS)

.cpp.o :
	$(CXX) $< $(INC) $(CFLAGS)

#삭제
clean :
	rm -f *.o $(TARGET) $(BACK) *.out *.txt *.log core*

#새로
new :
	clear
	$(MAKE) clean
	$(MAKE) $(TARGET)

#라이브러리 빌드
libs :
	$(CXX) $(SRCR) $(SRCS) $(INCR) $(CFLAGR)
	$(CXX) -o $(TARGET) *.o $(LPATHR) $(LFLAGR)

#디버깅
debug :
	$(CXX) $(SRCD) $(SRCS) $(INCD) $(CFLAGD)
	$(CXX) -o $(TARGET) *.o $(LPATHD) $(LFLAGD)

#프로파일링
prof :
	$(CXX) $(SRCP) $(SRCS) $(INCP) $(CFLAGP)
	$(CXX) -o $(TARGET) *.o $(LPATHP) $(LFLAGP)

#valgrind
valg :
	$(CXX) $(SRCV) $(SRCS) $(INCV) $(CFLAGV)
	$(CXX) -o $(TARGET) *.o $(LPATHV) $(LFLAGV)
