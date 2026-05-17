.DEFAULT_GOAL := build
.PHONY: all build rebuild clean distclean test debug run glfw pcg pcg_full superclean \
        cpplint cppcheck clang-format format

PROJECT_NAME := klotski
TARGET      := bin/$(PROJECT_NAME)
BUILDDIR    := $(abspath $(CURDIR)/build)

CC  := gcc
CXX := g++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ECHOFLAGS := -e
	LIBS := -lm -lglfw -lpthread -ldl -lstdc++ -lpcg_random -lGL
	LDFLAGS := -Ldeps/glfw/build/src -Ldeps/pcg-c/src
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS_EXTRA := -Wno-unused-command-line-argument -Wno-strict-prototypes
	CPPFLAGS_EXTRA := -Wno-unused-command-line-argument -Wno-mismatched-tags
	LIBS := -lm -lglfw -lpthread -ldl -lstdc++ -lpcg_random -framework OpenGL
	LDFLAGS := -Ldeps/glfw/build/src -Ldeps/pcg-c/src
endif

OPTIMIZATION := -O2

OPTIONS := -DIMGUI_IMPL_API="extern \"C\"" \
           -DIMGUI_IMPL_OPENGL_LOADER_GLAD

INCLUDES := -Isrc \
            -Ideps/glad/include \
            -Ideps/glfw/include \
            -Ideps/cglm/include \
            -Ideps/cimgui \
            -Ideps/cimgui/imgui \
            -Ideps/cimgui/imgui/backends \
            -Ideps/stb \
            -Ideps/pcg-c/include \
            -Ideps/pcg-c/extras \
            -Ideps/Unity/src

CFLAGS   := -Wall -Wextra -pedantic -Werror -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES) $(CFLAGS_EXTRA)
CFLAGS_THIRDPARTY := -Wall -Wno-use-after-free -std=gnu11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES)
CPPFLAGS := -Wall -std=c++11 $(OPTIMIZATION) $(OPTIONS) $(INCLUDES) $(CPPFLAGS_EXTRA)

# Graphics-dependent C files (not used in test binaries)
C_FILES_MAIN := src/main.c src/gui.c

# Headless-testable C files (no GL/GLFW headers)
C_FILES_TESTABLE := src/sample.c src/utils.c src/board.c

# All C files for main binary
C_FILES := $(C_FILES_MAIN) $(C_FILES_TESTABLE) \
           $(wildcard deps/pcg-c/extras/*.c) \
           deps/glad/src/glad.c \
           $(wildcard deps/stb/*.c)

# C++ files from src/
CPP_FILES := $(wildcard src/*.cpp)

# ImGui C++ files (cimgui + backends - only glfw + opengl3)
IMGUI_FILES := $(wildcard deps/cimgui/*.cpp) \
               $(wildcard deps/cimgui/imgui/*.cpp) \
               deps/cimgui/imgui/backends/imgui_impl_glfw.cpp \
               deps/cimgui/imgui/backends/imgui_impl_opengl3.cpp

SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(IMGUI_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

# Test object groups: testable C files + pcg extras + Unity src
C_TEST_DEPS := $(wildcard deps/pcg-c/extras/*.c) $(wildcard deps/Unity/src/*.c)
OBJS_TESTABLE := $(foreach src,$(C_FILES_TESTABLE:.c=.o), $(BUILDDIR)/$(src)) \
                 $(foreach src,$(C_TEST_DEPS:.c=.o), $(BUILDDIR)/$(src))

TEST_SRCS := $(wildcard test/test_*.c)
TEST_TARGETS := $(patsubst test/%.c,$(BUILDDIR)/test/%,$(TEST_SRCS))

all: build

build: glfw pcg pcg_full $(TARGET)

debug: OPTIMIZATION := -g -pg -O0
debug: build

rebuild: clean build

run: $(TARGET)
	LD_LIBRARY_PATH=deps/glfw/build/src $(CURDIR)/$(TARGET)

# GLFW cmake build (stamp file)
glfw: $(BUILDDIR)/.glfw_built

$(BUILDDIR)/.glfw_built:
	cmake -B deps/glfw/build -S deps/glfw -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_WAYLAND=OFF
	cmake --build deps/glfw/build -- -j$$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)
	mkdir -p $(BUILDDIR)
	touch $@

# PCG core library
pcg: $(BUILDDIR)/.pcg_core

$(BUILDDIR)/.pcg_core:
	$(MAKE) -s -C deps/pcg-c/src/
	mkdir -p $(BUILDDIR)
	touch $@

# PCG full build (extras)
pcg_full: $(BUILDDIR)/.pcg_full

$(BUILDDIR)/.pcg_full:
	$(MAKE) -s -C deps/pcg-c/
	mkdir -p $(BUILDDIR)
	touch $@

# Generic C compilation rule (with pedantic)
$(BUILDDIR)/%.o: %.c
	@mkdir -p "$(dir $@)"
	@printf "[CC]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

# Special rule for third-party code (glad, stb) with relaxed warnings
$(BUILDDIR)/deps/glad/src/glad.o: deps/glad/src/glad.c
	@mkdir -p "$(dir $@)"
	@printf "[CC]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CC) $(CFLAGS_THIRDPARTY) -o "$@" -c "$<"

$(BUILDDIR)/deps/stb/%.o: deps/stb/%.c
	@mkdir -p "$(dir $@)"
	@printf "[CC]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CC) $(CFLAGS_THIRDPARTY) -o "$@" -c "$<"

# Generic C++ compilation rule
$(BUILDDIR)/%.o: %.cpp
	@mkdir -p "$(dir $@)"
	@printf "[CXX]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CXX) $(CPPFLAGS) -o "$@" -c "$<"

# Main binary: link via gcc with -lstdc++ for C++ runtime
$(TARGET): $(OBJS)
	@mkdir -p "$(dir $@)"
	@printf "[LD]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CC) $(LDFLAGS) -o "$@" $^ $(LIBS)

# Test binaries: each test_*.c gets its own binary
# Link only testable objects (no GL/imgui), pcg core, and math
$(TEST_TARGETS): pcg
$(TEST_TARGETS): $(BUILDDIR)/test/%: $(BUILDDIR)/test/%.o $(OBJS_TESTABLE)
	@mkdir -p "$(dir $@)"
	@printf "[LD]\t$(subst $(CURDIR)/,,$@)\n"
	@$(CC) -g -o "$@" $(filter-out pcg,$^) -Ldeps/pcg-c/src -lpcg_random -lm

test: pcg $(TEST_TARGETS)
	@for t in $(TEST_TARGETS); do \
		echo $(ECHOFLAGS) "[RUN]\t$$t"; \
		$$t || exit $$?; \
	done
	@echo "All tests passed."

clean:
	@printf "[RM]\tsrc/\n"
	@rm -rf "$(BUILDDIR)/src"
	@printf "[RM]\ttest/\n"
	@rm -rf "$(BUILDDIR)/test"
	@printf "[RM]\tbin/\n"
	@rm -rf "bin"

distclean:
	@printf "[RM]\tbuild/\n"
	@rm -rf "$(BUILDDIR)"
	@rm -rf "bin"

superclean:
	@printf "[RM]\tbuild/\n"
	@rm -rf "$(BUILDDIR)"
	@printf "[RM]\tbin/\n"
	@rm -rf "bin"
	@printf "[RM]\tdeps/pcg-c/src/\n"
	@$(MAKE) -s -C deps/pcg-c/src/ clean 2>/dev/null || true
	@printf "[RM]\tdeps/pcg-c/\n"
	@$(MAKE) -s -C deps/pcg-c/ clean 2>/dev/null || true
	@printf "[RM]\tdeps/glfw/\n"
	@rm -rf deps/glfw/build

cpplint:
	cpplint --filter=-build/include_subdir,-readability/nolint,-whitespace/line_length,-whitespace/comments,-readability/casting,-build/header_guard,-runtime/arrays src/*.c

cppcheck:
	cppcheck --enable=all --suppressions-list=.cppcheck.suppressions --std=c11 --language=c --error-exitcode=1 src/*.c

clang-format:
	./format.sh

format:
	./format.sh

.PRECIOUS: $(BUILDDIR)/%.o
