# Makefile for ImageLib

DIR_INC = include
DIR_LIB = lib
DIR_SRC = src
DIR_OBJ = obj

IMAGELIB = $(DIR_LIB)/libImage.a
IMAGELIB_OBJS = $(DIR_OBJ)/Convert.o \
								$(DIR_OBJ)/Convolve.o \
								$(DIR_OBJ)/FileIO.o \
								$(DIR_OBJ)/Image.o \
								$(DIR_OBJ)/ImageProc.o \
								$(DIR_OBJ)/Pyramid.o \
								$(DIR_OBJ)/RefCntMem.o \
								$(DIR_OBJ)/Transform.o \
								$(DIR_OBJ)/WarpImage.o

CC = g++
CPPFLAGS = -Wall -O3 -I$(DIR_INC)

all: $(IMAGELIB)

$(IMAGELIB): $(IMAGELIB_OBJS)
	ar r $(IMAGELIB) $(IMAGELIB_OBJS)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -f $(DIR_OBJ)/*.o
	rm -f $(IMAGELIB)
