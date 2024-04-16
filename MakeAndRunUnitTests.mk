#This makefile makes all the main book code with CppUTest test harness

#Set this to @ to keep the makefile quiet
SILENCE = @

#---- Outputs ----#
COMPONENT_NAME = Device_Monitor

CPPUTEST_USE_GCOV=Y

#--- Inputs ----#
CPP_PLATFORM = g++
PROJECT_HOME_DIR = .

SRC_DIRS = \
	src/Socket \
	src/SSLsocket\

TEST_SRC_DIRS = \
	.\
	mocks \
	tests\
	tests/Socket \
	tests/SSLsocket\

	
INCLUDE_DIRS =\
  .\
  $(CPPUTEST_HOME)/include\
  include/Socket \
  include/SSLsocket\


MOCKS_SRC_DIRS = \
	mocks\
	
CPPUTEST_WARNINGFLAGS = -Wall -Wswitch-default -Werror 
CPPUTEST_CFLAGS = -std=c++14 
CPPUTEST_CFLAGS += -Wall -Wstrict-prototypes -pedantic
LD_LIBRARIES = -lpthread  -lssl  -lcrypto
	
  
ifeq ($(CPPUTEST_HOME),)
$(info CPPUTEST_HOME not set! See README.txt)
else  
include $(CPPUTEST_HOME)/build/MakefileWorker.mk
endif
