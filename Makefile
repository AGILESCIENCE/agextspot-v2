#############################################################################
# Use make variable_name=' options ' to override the variables or make -e to
# override the file variables with the environment variables
#	make CXXFLAGS='-g'
#	make prefix='/usr'
#	make CXX=g++
# External environment variable: CFISIO, ROOTSYS, CTARTA, ICEDIR
# Instructions:
# - modify the section 1)
# - in section 10), modify the following action:
#	* all: and or remove exe and lib prerequisite
#	* lib: and or remove staticlib and dynamiclib prerequisite
#	* clean: add or remove the files and directories that should be cleaned
#	* install: add or remove the files and directories that should be installed
#	* uninstall: add or remove the files and directories that should be uninstalled
#############################################################################

PROJECT= AG_extspot-v2
SHELL = /bin/sh

####### 0) PREREQUISITES
ifndef AGILE
$(error AGILE is not set.)
endif
ifndef CFITSIO
$(error CFITSIO is not set.)
endif
ifndef OPENCV
$(error OPENCV is not set.)
endif



####### 1) Project names and system

SYSTEM= $(shell gcc -dumpmachine)
#ice, ctarta, mpi, cfitsio
LINKERENV= pil, agile, opencv, cfitsio, root, wcs, healpix

# Applications
AG_EXTSPOT = AG_extspot-v2
AG_EXTSPOT-PERFORMANCE-EVALUATOR = AG_extspot_v2_performance_evaluator
AG_EXTSPOT-BAYESIAN-MODEL-EVALUATOR = AG_extspot_v2_bayesian_model_evaluator
AG_EXTSPOT-BLOBS-LIST-GENERATOR = AG_extspot_v2_blobs_list_generator

VER_FILE_NAME = version.h
#the name of the directory where the conf file are copied (into $(datadir))
CONF_DEST_DIR = conf
#the name of the icon for the installation
ICON_NAME=

####### 2) Directories for the installation

# Prefix for each installed program. Only ABSOLUTE PATH
prefix=$(AGILE)
exec_prefix=$(prefix)
# The directory to install the binary files in.
bindir=$(exec_prefix)/bin
# The directory to install the local configuration file.
datadir=$(exec_prefix)/share
# The directory to install the libraries in.
# The directory to install the include files in.
includedir=$(exec_prefix)/include
# The directory to install the icon
icondir=$(HOME)/.local/share/applications/

####### 3) Directories for the compiler

OBJECTS_DIR = obj
SOURCE_DIR = src
INCLUDE_DIR = include
PROJECTS_DIR = projects
EXE_DESTDIR  = bin
CONF_DIR = conf
ICON_DIR = ui

####### 4) Compiler, tools and options

ifneq (, $(findstring mpi, $(LINKERENV)))
CXX = mpic++
else
CXX = g++
endif

CXXFLAGS = -g -std=c++11 -O2 -pipe -I $(INCLUDE_DIR)

LIBS += -lm

ifneq (, $(findstring agile, $(LINKERENV)))
    ifeq (, $(findstring -I $(AGILE)/include, $(CXXFLAGS)))
        CXXFLAGS += -I $(AGILE)/include
    endif
    LIBS += -L$(AGILE)/lib -lagilesci
endif
ifneq (, $(findstring wcs, $(LINKERENV)))
    ifeq (,$(findstring -I $(AGILE)/include, $(CXXFLAGS)))
        CXXFLAGS += -I $(AGILE)/include
    endif
    LIBS += -L$(AGILE)/lib -lagilewcs
endif
ifneq (, $(findstring opencv, $(LINKERENV)))
    ifeq (, $(findstring -I $(OPENCV)/include, $(CXXFLAGS)))
        CXXFLAGS += -I $(OPENCV)/include
    endif
    LIBS += -L$(OPENCV)/lib -lopencv_core -lopencv_highgui -lopencv_imgproc
endif
ifneq (, $(findstring pil, $(LINKERENV)))
    ifeq (,$(findstring -I $(AGILE)/include, $(CXXFLAGS)))
        CXXFLAGS += -I $(AGILE)/include
    endif
    LIBS += -L$(AGILE)/lib -lagilepil
endif
ifneq (, $(findstring root, $(LINKERENV)))
    CXXFLAGS += -W -fPIC -D_REENTRANT $(shell root-config --cflags)
    LIBS += $(shell root-config --glibs) -lMinuit
endif
ifneq (, $(findstring cfitsio, $(LINKERENV)))
    CXXFLAGS += -I$(CFITSIO)/include
    LIBS += -L$(CFITSIO)/lib -lcfitsio
endif
ifneq (, $(findstring pil, $(LINKERENV)))
    ifeq (,$(findstring -I $(AGILE)/include, $(CXXFLAGS)))
        CXXFLAGS += -I $(AGILE)/include
    endif
    LIBS += -L$(AGILE)/lib -lagilepil
endif
ifneq (, $(findstring healpix, $(LINKERENV)))
    ifeq (,$(findstring -I $(HEALPIX_INCDIR), $(CXXFLAGS)))
        CXXFLAGS += -I $(HEALPIX_INCDIR)
    endif
    LIBS += -L$(HEALPIX_LIBDIR) -lhealpix_cxx -lcxxsupport -lsharp -lfftpack -lc_utils -lcfitsio#-lsharp -lfftpack -lcxxsupport -lc_utils
endif

CXXFLAGS += -I rapidjson


AR       = ar cqs
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f -r
COPY_FILE= $(COPY) -p
COPY_DIR = $(COPY) -pR
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rm -rf
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p


#######  VPATH
VPATH=$(SOURCE_DIR):$(INCLUDE_DIR):
vpath %.o $(OBJECTS_DIR)

INCLUDE=$(foreach dir,$(INCLUDE_DIR), $(wildcard $(dir)/*.h))
SOURCE=$(foreach dir,$(SOURCE_DIR), $(wildcard $(dir)/*.cpp))
OBJECTS=$(addsuffix .o, $(basename $(notdir $(SOURCE))))





# Pattern rule
#%.o : $(SOURCE_DIR)/%.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJECTS_DIR)/$@



#all: compile the entire program.
all: makeobjdir agextspot performance bayesian

makeobjdir:
	test -d $(OBJECTS_DIR) || mkdir -p $(OBJECTS_DIR)

agextspot: $(PROJECTS_DIR)/AG_extspot-v2.cpp $(OBJECTS)
	test -d $(EXE_DESTDIR) || mkdir -p $(EXE_DESTDIR)
	$(CXX) $(CXXFLAGS) $(PROJECTS_DIR)/AG_extspot-v2.cpp $(OBJECTS_DIR)/*.o -o $(EXE_DESTDIR)/$(AG_EXTSPOT) $(LIBS)

performance: $(PROJECTS_DIR)/AG_extspot-v2-performance-evaluator.cpp $(OBJECTS)
	test -d $(EXE_DESTDIR) || mkdir -p $(EXE_DESTDIR)
	$(CXX) $(CXXFLAGS) $(PROJECTS_DIR)/AG_extspot-v2-performance-evaluator.cpp $(OBJECTS_DIR)/*.o -o $(EXE_DESTDIR)/$(AG_EXTSPOT-PERFORMANCE-EVALUATOR) $(LIBS) #$(INCLUDE_DIR)/PerformanceEvaluator.h $(SOURCE_DIR)/PerformanceEvaluator.cpp
	$(COPY_FILE) $(SOURCE_DIR)/draw_performance_plot.py $(EXE_DESTDIR)/draw_performance_plot.py

bayesian: $(PROJECTS_DIR)/AG_extspot-v2-bayesian-model-evaluator.cpp $(OBJECTS)
	test -d $(EXE_DESTDIR) || mkdir -p $(EXE_DESTDIR)
	$(CXX) $(CXXFLAGS) $(PROJECTS_DIR)/AG_extspot-v2-bayesian-model-evaluator.cpp $(OBJECTS_DIR)/*.o -o $(EXE_DESTDIR)/$(AG_EXTSPOT-BAYESIAN-MODEL-EVALUATOR) $(LIBS) # $(INCLUDE_DIR)/BayesianModelEvaluator.h $(SOURCE_DIR)/BayesianModelEvaluator.cpp

blobslistgen:  $(PROJECTS_DIR)/AG_extspot_v2_blobs_list_generator.cpp $(OBJECTS)
	test -d $(EXE_DESTDIR) || mkdir -p $(EXE_DESTDIR)
	$(CXX) $(CXXFLAGS) $(PROJECTS_DIR)/AG_extspot_v2_blobs_list_generator.cpp $(OBJECTS_DIR)/*.o -o $(EXE_DESTDIR)/$(AG_EXTSPOT-BLOBS-LIST-GENERATOR) $(LIBS)




#clean: delete all files from the current directory that are normally created by building the program.
clean:
	$(DEL_FILE) $(OBJECTS_DIR)/*.o
	$(DEL_FILE) $(EXE_DESTDIR)/AG_*


#Delete all files from the current directory that are created by configuring or building the program.
distclean: clean

#install: compile the program and copy the executables, libraries,
#and so on to the file names where they should reside for actual use.
install: all

	# For exe installation
	test -d $(bindir) || mkdir -p $(bindir)
	$(COPY_FILE) $(EXE_DESTDIR)/* $(bindir)

	# For conf files installation
	test -d $(datadir) || mkdir -p $(datadir)
	$(COPY_FILE) $(CONF_DIR)/* $(datadir)/$(CONF_DEST_DIR)


#uninstall: delete all the installed files--the copies that the `install' target creates.
uninstall:
	# For exe uninstall
	$(DEL_FILE) $(bindir)/$(EXE_NAME)
