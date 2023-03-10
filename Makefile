
build_platform=native
# build_platform=web-wasm

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native)
# $(info build_platform is valid, value=$(build_platform))
else ifeq ($(build_platform),web-wasm)
# $(info build_platform is valid, value=$(build_platform))
else
$(error unsupported value for "build_platform", value=$(build_platform))
endif

#

ifeq ($(build_mode),release)
# $(info build_mode is valid, value=$(build_mode))
else ifeq ($(build_mode),debug)
# $(info build_mode is valid, value=$(build_mode))
else
$(error unsupported value for "build_mode", value=$(build_mode))
endif




DIR_LIB_GERONIMO?= ./thirdparties/dependencies/geronimo



LOG_INFO= '[$(build_mode)] [$(build_platform)]'

#

ifeq ($(build_platform),native)

DIR_TARGET=			./lib/native
NAME_TARGET=		$(DIR_TARGET)/lib-basic-genetic-algorithm.a

else

DIR_TARGET=			./lib/web-wasm
NAME_TARGET=		$(DIR_TARGET)/lib-basic-genetic-algorithm.bc

endif



#### DIRS

DIR_SRC=											./src
$(DIR_LIB_GERONIMO)/thirdparties=

#### /DIRS



ifeq ($(build_platform),native)

DIR_OBJ=	./obj/native

else

DIR_OBJ=	./obj/web-wasm

endif

#### SRC

SRC_FILES+=	\
	$(wildcard \
		$(DIR_SRC)/basic-genetic-algorithm/*.cpp \
		$(DIR_SRC)/basic-genetic-algorithm/experimental/*.cpp \
		)

#

OBJ_FILES=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC_FILES))

#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3 # optimisation flag

else

BUILD_FLAG=		-g3 # debug flag

endif

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/thirdparties

ifeq ($(build_platform),native)

CXX=clang++
AR=ar

else

CXX=em++
AR=emar

endif


RM=			rm -rf


#######

#
## RULE(S)

all:	basic-genetic-algorithm

ensurefolders:
	@mkdir -p `dirname $(NAME_TARGET)`

# @mkdir -p $(dir $@)

basic-genetic-algorithm:	ensurefolders $(OBJ_FILES)
	@echo ' ---> building $(LOG_INFO): "basic genetic algorithm library"'
	@$(AR) cr $(NAME_TARGET) $(OBJ_FILES)
	@echo '   --> built $(LOG_INFO): "basic genetic algorithm library"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ) && find $(DIR_OBJ) -name "*.dep")

#

clean:
	@echo ' -> cleaning $(LOG_INFO): basic genetic algorithm build file(s)'
	$(RM) $(DIR_OBJ)
	@echo '   -> cleaned $(LOG_INFO): basic genetic algorithm build file(s)'

fclean:	clean
	@echo ' -> cleaning $(LOG_INFO): basic genetic algorithm file(s)'
	$(RM) $(NAME_TARGET)
	@echo '   -> cleaned $(LOG_INFO): basic genetic algorithm file(s)'

re:	fclean all

.PHONY: \
			all \
			basic-genetic-algorithm \
			clean \
			fclean \
			re

## RULE(S)
#
