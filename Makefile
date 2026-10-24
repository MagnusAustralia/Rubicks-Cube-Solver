CXX      := clang++
CXXFLAGS := -Wall -std=c++17 -MMD -MP
CPPFLAGS ?=
LDFLAGS  ?=
LDLIBS   ?=

CLANG_FORMAT ?= clang-format
CLANG_TIDY   ?= clang-tidy

SRC_DIR     := src
INCLUDE_DIR := include
BUILD_DIR   := build

SRCS    := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

# raylib (macOS, installed with `brew install raylib`)
RAYLIB_PREFIX   := $(shell brew --prefix raylib 2>/dev/null)
RAYLIB_CPPFLAGS := -I$(RAYLIB_PREFIX)/include
RAYLIB_LDLIBS   := -L$(RAYLIB_PREFIX)/lib -lraylib \
                   -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# FILE = the file whose main() runs. DEPS = the other files to link with it.
# If DEPS isn't set (e.g. `make run FILE=x`), every other source file is linked.
FILE ?= depth_first_search
DEPS ?= $(filter-out $(FILE),$(basename $(notdir $(SRCS))))
A    ?=

OBJ_DIR := $(BUILD_DIR)/obj/$(FILE)
OBJS    := $(patsubst %,$(OBJ_DIR)/%.o,$(FILE) $(DEPS))
TARGET  := $(BUILD_DIR)/$(FILE)

.DEFAULT_GOAL := dfs
.PHONY: dfs 2x2 3x3 lehmer gen-corners gen-edges render run check check-raylib clean format format-check tidy lint

# ---- Shortcuts: FILE + only the files it depends on ----
dfs:         ; @$(MAKE) --no-print-directory run FILE=depth_first_search DEPS="generate_data lehmer_code rubicks_2x2 rubicks_3x3 generate_scramble" A="$(A)"
2x2:         ; @$(MAKE) --no-print-directory run FILE=rubicks_2x2 DEPS="" A="$(A)"
3x3:         ; @$(MAKE) --no-print-directory run FILE=rubicks_3x3 DEPS="rubicks_2x2" A="$(A)"
lehmer:      ; @$(MAKE) --no-print-directory run FILE=lehmer_code DEPS="rubicks_2x2" A="$(A)"
gen-corners: ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="corners $(A)"
gen-edges:   ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="edges $(A)"

# Build and run render.cpp with raylib. Override DEPS if render only needs some files,
# e.g. `make render DEPS="rubicks_2x2 rubicks_3x3"`.
render: check-raylib
	@$(MAKE) --no-print-directory run FILE=renderer \
		$(if $(DEPS_SET),DEPS="$(DEPS)") \
		CPPFLAGS="$(RAYLIB_CPPFLAGS)" LDLIBS="$(RAYLIB_LDLIBS)" A="$(A)"

check-raylib:
	@test -n "$(RAYLIB_PREFIX)" || { echo "error: raylib not found (install with: brew install raylib)"; exit 1; }

# ---- Build and run ----
run: check $(TARGET)
	./$(TARGET) $(A)

check:
	@test -f $(SRC_DIR)/$(FILE).cpp || { echo "error: $(SRC_DIR)/$(FILE).cpp does not exist"; exit 1; }

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Safety net: any linked file other than $(FILE) gets its main() renamed
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(if $(filter $*,$(FILE)),,-Dmain=main_$*) -c $< -o $@

# ---- Formatting and linting ----
# Reformat all sources and headers in place (uses .clang-format)
format:
	$(CLANG_FORMAT) -i $(SRCS) $(HEADERS)

# Fail if anything isn't formatted, without changing files (good for CI)
format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(SRCS) $(HEADERS)

# Static analysis on every source file (uses .clang-tidy)
tidy:
	$(CLANG_TIDY) $(SRCS) -- -std=c++17 -I$(INCLUDE_DIR)

# Run both checks
lint: format-check tidy

clean:
	rm -rf $(BUILD_DIR)/obj
	find $(BUILD_DIR) -maxdepth 1 -type f -delete

-include $(OBJS:.o=.d)