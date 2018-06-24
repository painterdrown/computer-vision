# Makefile for project 2

DIR_INC = include
DIR_SRC = src
DIR_OBJ = obj
BIN_LIB = bin

PROJ1 = $(BIN_LIB)/Features
PROJ2 = $(BIN_LIB)/Panorama
PROJ1_OBJS = $(DIR_OBJ)/FeatureSet.o \
						 $(DIR_OBJ)/ImageDatabase.o \
						 $(DIR_OBJ)/ImageView.o \
						 $(DIR_OBJ)/FeaturesMain.o \
						 $(DIR_OBJ)/Features.o
PROJ2_OBJS = $(DIR_OBJ)/Panorama.o \
						 $(DIR_OBJ)/BlendImages.o \
						 $(DIR_OBJ)/FeatureAlign.o \
						 $(DIR_OBJ)/FeatureSet.o \
						 $(DIR_OBJ)/WarpSpherical.o

IMAGELIB = ImageLib/lib/libImage.a

CC = g++
CPPFLAGS = -Wall -O3 -I/usr/include -I$(DIR_INC)
LIB_PATH = -L/usr/lib -L/usr/X11R6/lib
LIBS = -lfltk -lfltk_images -lpng -ljpeg -lX11 -lXext -lXi -lXmu

all: $(PROJ1) $(PROJ2)

$(IMAGELIB):
	make -C ImageLib

$(PROJ1): $(PROJ1_OBJS) $(IMAGELIB)
	$(CC) -o $@ $(PROJ1_OBJS) $(LIB_PATH) $(LIBS) $(IMAGELIB)

$(PROJ2): $(PROJ2_OBJS) $(IMAGELIB)
	$(CC) -o $@ $(PROJ2_OBJS) $(LIB_PATH) $(LIBS) $(IMAGELIB)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -f $(DIR_OBJ)/*.o
	rm -f $(PROJ1) $(PROJ2)
	make clean -C ImageLib
