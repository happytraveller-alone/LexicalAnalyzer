CC = g++
CXXFLAGS = -Wall -fexec-charset=UTF-8 -fexec-charset=UTF-8 -std=c++11
TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src
DIRS = $(shell find $(SRC_PATH) -maxdepth 3 -type d)
SRCS_CPP += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJS_CPP = $(patsubst %.cpp, %.o, $(src))
target = project
objects = project.o  LexAnalysis.o SynAnalysis.o
$(target) : $(objects)
	$(CC) $(CXXFLAGS) $^ -o $@

$(objects) : %.o : %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@
# clean: del $(objects)
# oclean
.PHONY:clean
clean:
	del $(objects) project.exe
oclean:
	del $(objects)