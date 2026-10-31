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

CPPFLAGS += -I$(INCLUDE_DIR)

SRCS    := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

FILE ?= menu_cli
DEPS ?= $(filter-out $(FILE),$(basename $(notdir $(SRCS))))
A    ?=

OBJ_DIR := $(BUILD_DIR)/obj/$(FILE)
OBJS    := $(patsubst %,$(OBJ_DIR)/%.o,$(FILE) $(DEPS))
TARGET  := $(BUILD_DIR)/$(FILE)

.DEFAULT_GOAL := menu_cli
.PHONY: cli korf kociemba 2x2 3x3 lehmer \
        gen-corners gen-edges-1 gen-edges-2 gen-corners-slices \
        gen-edges-slices gen-corners-slices-perm gen-edges-slices-perm \
        run check clean format format-check tidy lint

# ---- Shortcuts ----
menu_cli:                        ; @$(MAKE) --no-print-directory run FILE=menu_cli A="$(A)"
korf:                       ; @$(MAKE) --no-print-directory run FILE=korfs_algorithm DEPS="depth_first_search generate_data lehmer_code rubicks_2x2 rubicks_3x3 generate_scramble" A="$(A)"
kociemba:                   ; @$(MAKE) --no-print-directory run FILE=kociembas_algorithm DEPS="depth_first_search generate_data lehmer_code rubicks_2x2 rubicks_3x3 generate_scramble" A="$(A)"
2x2:                        ; @$(MAKE) --no-print-directory run FILE=rubicks_2x2 DEPS="" A="$(A)"
3x3:                        ; @$(MAKE) --no-print-directory run FILE=rubicks_3x3 DEPS="rubicks_2x2" A="$(A)"
lehmer:                     ; @$(MAKE) --no-print-directory run FILE=lehmer_code DEPS="rubicks_2x2 rubicks_3x3" A="$(A)"
gen-corners:                ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="corners $(A)"
gen-edges-1:                ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="edges-1 $(A)"
gen-edges-2:                ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="edges-2 $(A)"
gen-corners-slices:         ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="corners-slice $(A)"
gen-edges-slices:           ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="edges-slice $(A)"
gen-corners-slices-perm:    ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="corners-slices-perm $(A)"
gen-edges-slices-perm:      ; @$(MAKE) --no-print-directory run FILE=generate_data DEPS="lehmer_code rubicks_2x2 rubicks_3x3" A="edges-slices-perm $(A)"

# ---- Build and run ----
run: check $(TARGET)
	./$(TARGET) $(A)

check:
	@test -f $(SRC_DIR)/$(FILE).cpp || { echo "error: $(SRC_DIR)/$(FILE).cpp does not exist"; exit 1; }

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Any linked file other than $(FILE) gets its main() renamed
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(if $(filter $*,$(FILE)),,-Dmain=main_$*) -c $< -o $@

# ---- Formatting and linting ----
format:
	$(CLANG_FORMAT) -i $(SRCS) $(HEADERS)

format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(SRCS) $(HEADERS)

tidy:
	$(CLANG_TIDY) $(SRCS) -- -std=c++17 -I$(INCLUDE_DIR)

lint: format-check tidy

clean:
	rm -rf $(BUILD_DIR)/obj
	-find $(BUILD_DIR) -maxdepth 1 -type f -delete

-include $(OBJS:.o=.d)