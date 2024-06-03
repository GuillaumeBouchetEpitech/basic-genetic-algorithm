
DIR_LIB_GERONIMO?= ../thirdparties/dependencies/geronimo


DIR_APPLICATION=								./bin
DIR_SRC=												./src
DIR_OBJ=												./obj
DIR_LIB_BASIC_GENTIC_ALGORITHM=	..

#
#
#

NAME_APPLICATION=	$(DIR_APPLICATION)/exec

NAME_GERONIMO=		$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-system.a

NAME_LIB_BASIC_GENTIC_ALGORITHM=	$(DIR_LIB_BASIC_GENTIC_ALGORITHM)/lib/native/lib-basic-genetic-algorithm.a

#
#
#

SRC+= \
	$(wildcard \
		$(DIR_SRC)/genetic_algorithm/*.cpp \
	)

SRC+= \
	$(wildcard \
		$(DIR_SRC)/continuous_genetic_algorithm/*.cpp \
	)

OBJ=		$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))



BUILD_FLAG= -O3
# BUILD_FLAG= -g3

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++20
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_LIB_BASIC_GENTIC_ALGORITHM)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/thirdparties
CXXFLAGS += -no-pie

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS += $(NAME_GERONIMO)

DEPENDENCIES_LIST += gtest_main

CXXFLAGS +=	$(shell pkg-config $(DEPENDENCIES_LIST) --cflags)
LDFLAGS +=	$(shell pkg-config $(DEPENDENCIES_LIST) --libs)


RM=			rm -rf


#######

#
## RULE(S)

all:	application

ensurefolders:
	@mkdir -p `dirname $(NAME_APPLICATION)`

application:	ensurefolders $(OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO) "application"'

#

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS) $< -c -o $@

#

clean:
	@echo ' -> cleaning $(LOG_INFO): application build file(s)'
	@$(RM) $(DIR_OBJ)
	@echo '   -> cleaned $(LOG_INFO): application build file(s)'

fclean:	clean
	@echo ' -> cleaned $(LOG_INFO): application file(s)'
	@$(RM) $(NAME_APPLICATION)
	@echo '   -> cleaned $(LOG_INFO): application file(s)'

re:			fclean all

.PHONY:	\
	all \
	ensurefolders \
	application \
	clean \
	fclean \
	re

## RULE(S)
#
