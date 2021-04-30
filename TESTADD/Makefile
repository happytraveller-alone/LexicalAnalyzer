# 使用文档：
# 可使用命令为 make,make oclean,make clean
# make 进行文件的编译运行，生成可执行文件
# make clean 删除预编译.o文件核,可执行文件,输出的文本文件
# make oclean 删除预编译.o文件，输出的文本文件

CC = g++ #C++编译
CXXFLAGS = -Wall -fexec-charset=UTF-8 -fexec-charset=UTF-8 -std=c++11 #显示警告，输入编码UTF-8,
TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src
DIRS = $(shell find $(SRC_PATH) -maxdepth 3 -type d)
SRCS_CPP += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJS_CPP = $(patsubst %.cpp, %.o, $(src))
target = project
objects = src\\project.o  src\\LexAnalysis.o src\\SynAnalysis.o
output = OutputFile\\*.txt
$(target) : $(objects)
	$(CC) $(CXXFLAGS) $^ -o $@

$(objects) : %.o : %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	del $(objects) $(output) project.exe
oclean:
	del $(objects) $(output)
