## log module   makefile ##
DEBUG_FLAG = -g -ggdb3
CC=g++ -rdynamic
CPPFLAGS=-Wall -O0 $(DEBUG_FLAG) -D__LINUX__
SRC_PATH=./
INC=-I. \
	-I/home/zhouwenlong/libs/log4cplus/include \
	-I/home/zhouwenlong/protobuf/include
BUILD=./objs/
OBJ_PATH=./objs
TARGET=out.exe


## No need modify ##
SRC_FILE=$(shell cd $(SRC_PATH)&&echo *.cpp)
SRC:=$(foreach n,$(SRC_FILE),$(SRC_PATH)/$(n))
OBJ_FILE:=$(SRC_FILE:.cpp=.o)
LIBS=-L./ \
	-L/home/zhouwenlong/libs/log4cplus/lib \
	-L/home/zhouwenlong/protobuf/lib
USELIBS=$(LIBS) -llog4cplus -lpthread /home/zhouwenlong/protobuf/lib/libprotobuf.a
OBJ:=$(foreach n,$(OBJ_FILE),$(OBJ_PATH)/$(n))
DEP_FILE:=$(SRC_FILE:.cpp=.d)
DEP:=$(foreach n,$(DEP_FILE),$(OBJ_PATH)/$(n))

all:$(TARGET)

.PHONY=all clean cleanall

$(DEP):$(OBJ_PATH)/%.d:$(SRC_PATH)/%.cpp
	mkdir -p $(OBJ_PATH)
	$(CC) -MM $(CPPFLAGS) $< $(INC) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEP) 

$(TARGET):$(OBJ)
	$(CC) -o $@ -ldl $(OBJ) $(USELIBS) $(USEIPPLIBS)
	-cp $@ $(BUILD)

$(OBJ):$(OBJ_PATH)/%.o:$(SRC_PATH)/%.cpp
	$(CC) $(CPPFLAGS) -c $<  $(INC) -o $@   
clean:
	-rm -f $(DEP)
	-rm -f $(OBJ)
	-rm -f $(TARGET)
	-rm -f $(BUILD)/$(TARGET)

cleanall:
	-rm -f $(DEP)
	-rm -f $(OBJ)
	-rm -f $(TARGET)


