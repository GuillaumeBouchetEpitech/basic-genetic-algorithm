
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

ifeq ($(build_mode),release)

# $(info build_mode is valid, value=$(build_mode))

else ifeq ($(build_mode),debug)

# $(info build_mode is valid, value=$(build_mode))

else

$(error unsupported value for "build_mode", value=$(build_mode))

endif

LOG_INFO= '[$(build_mode)] [$(build_platform)]'

#

DIR_SRC=		src

#
#

ifeq ($(build_platform),native)

DIR_APPLICATION=	./bin

else ifeq ($(build_platform),web-wasm)

DIR_APPLICATION=	./dist/wasm

endif

#
#

DIR_LIB_BASIC_GENETIC_ALGORITHM=	../..

DIR_DEPENDENCIES=								../../thirdparties/dependencies
DIR_LIB_GERONIMO=								${DIR_DEPENDENCIES}/geronimo

ifeq ($(build_platform),native)

NAME_APPLICATION=		$(DIR_APPLICATION)/exec

NAME_LIB_BASIC_GENTIC_ALGORITHM=	$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/lib/native/lib-basic-genetic-algorithm.a

NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-audio.a
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-physics.a
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-graphics.a
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-system.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-dynamics.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-collision.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-linearmath.a

else ifeq ($(build_platform),web-wasm)

NAME_APPLICATION=				$(DIR_APPLICATION)/index.js

NAME_LIB_BASIC_GENTIC_ALGORITHM=	$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/lib/web-wasm/lib-basic-genetic-algorithm.bc

NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-audio.bc
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-physics.bc
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-graphics.bc
NAME_LIB_GERONIMO+=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-system.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-linearmath.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-collision.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-dynamics.bc

endif



#### DIRS

#### /DIRS



ifeq ($(build_platform),native)

DIR_OBJ=	./obj/native

else ifeq ($(build_platform),web-wasm)

DIR_OBJ=	./obj/web-wasm

endif



#### SRC

# common src
SRC+= \
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/application/*.cpp \
		$(DIR_SRC)/application/context/*.cpp \
		$(DIR_SRC)/application/states/*.cpp \
	)

SRC+= \
	$(wildcard \
		$(DIR_SRC)/application/context/logic/*.cpp \
	)

SRC+= \
	$(wildcard \
		$(DIR_SRC)/application/context/graphics/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/geometries-stack-renderer/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/geometries-stack-renderer/internals/*.cpp \
	)

SRC+= \
	$(wildcard \
		$(DIR_SRC)/application/context/logic/controllers/*.cpp \
		$(DIR_SRC)/application/context/logic/compound-shape/*.cpp \
		$(DIR_SRC)/application/context/logic/input-managers/*.cpp \
		$(DIR_SRC)/application/context/logic/inverse-kinematic/*.cpp \
		$(DIR_SRC)/application/context/logic/entities/*.cpp \
		$(DIR_SRC)/application/context/logic/entities/humanoids/model/*.cpp \
		$(DIR_SRC)/application/context/logic/entities/humanoids/controller/*.cpp \
		$(DIR_SRC)/application/context/logic/proto-navmesh/*.cpp \
	)

# $(DIR_SRC)/application/context/graphics/renderers/scene/particles/*.cpp \
# $(DIR_SRC)/application/context/graphics/renderers/scene/particles/internals/*.cpp \

#

OBJ=		$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

#

#

#### /BULLET_SRC



#######


ifeq ($(build_mode),release)

BUILD_FLAG= -O3

else

BUILD_FLAG= -g3

endif

LIBRARIES += sdl2 glesv2 openal

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++20
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion
# CXXFLAGS += -Werror
CXXFLAGS += -I$(DIR_SRC)
# CXXFLAGS += -I$(DIR_LIB_GERONIMO)/src
# CXXFLAGS += -I$(DIR_LIB_GERONIMO)/thirdparties
# CXXFLAGS += -I${DIR_DEPENDENCIES}
# CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/thirdparties


# CXXFLAGS += -DBT_NO_PROFILE
# CXXFLAGS += -DSIMD_FORCE_INLINE=inline
# # CXXFLAGS += -DBT_USE_DOUBLE_PRECISION


LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS += $(NAME_LIB_GERONIMO)
LDFLAGS += $(NAME_LIB_BULLET_PHYSICS)

CXXFLAGS += $(shell pkg-config $(LIBRARIES) --cflags)

LDFLAGS += $(shell pkg-config $(LIBRARIES) --libs)

ifeq ($(build_platform),native)

CXX=g++
AR=ar

else ifeq ($(build_platform),web-wasm)

CXX=em++
AR=emar

CXXFLAGS += -s USE_PTHREADS=0

LDFLAGS+= -s GL_PREINITIALIZED_CONTEXT=1

LDFLAGS += -s USE_SDL=2
LDFLAGS += -s USE_WEBGL2=1
LDFLAGS += -s FULL_ES3=1
LDFLAGS += -s WASM=1
LDFLAGS += -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
LDFLAGS += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
LDFLAGS += -s EXPORTED_RUNTIME_METHODS=cwrap

ifeq ($(build_mode),debug)

# LDFLAGS_COMMON_WEB_WASM += -s DEMANGLE_SUPPORT=1
# LDFLAGS_COMMON_WEB_WASM += -s GL_ASSERTIONS=1
# LDFLAGS_COMMON_WEB_WASM += -s GL_DEBUG=1
LDFLAGS_COMMON_WEB_WASM += -s NO_DISABLE_EXCEPTION_CATCHING
LDFLAGS_COMMON_WEB_WASM += -s ASSERTIONS

endif

LDFLAGS+=	--preload-file ./assets/
LDFLAGS+=	--preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/textRenderer/assets
LDFLAGS+=	--preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/stackRenderers/shaders

LDFLAGS += -s EXPORT_NAME="selfDriving3dCars"
LDFLAGS += -s MODULARIZE=1

LDFLAGS+=	-s TOTAL_MEMORY=128Mb


ifeq ($(build_mode),debug)

LDFLAGS+=	-s DEMANGLE_SUPPORT=1
LDFLAGS+=	-s GL_ASSERTIONS=1
LDFLAGS+=	-s GL_DEBUG=1

endif


endif


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

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

# # pull in dependency info for *existing* .o files
# -include $(OBJ:.o=.o.d)

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ) && find $(DIR_OBJ) -name "*.dep")

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
