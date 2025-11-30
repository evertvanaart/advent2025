CXX_FLAGS := -Wall -Wextra -std=c++23 -ggdb
PRE_FLAGS := -MMD -MP

# When running or profiling, use the first argument to determine the target day
# and part, and only build the solution files corresponding to that day and part
# (i.e., solve_x.cpp and common.cpp), plus the main file. This keeps compilation
# time down, and ensures that solutions can be profiled in isolation.

ifneq (, $(filter $(firstword $(MAKECMDGOALS)),run profile))
    RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
    DAY_PART := $(word 1,$(RUN_ARGS))
    DAY  := $(shell echo ${DAY_PART} | cut -c 1-2)
    PART := $(shell echo ${DAY_PART} | cut -c 3-3)
    $(eval $(RUN_ARGS):;@:)
endif

SRCS := src/main.cpp src/solutions/$(DAY)/common.cpp src/solutions/$(DAY)/solve_$(PART).cpp
OBJS := $(subst src/,build/,$(addsuffix .o,$(basename $(SRCS))))
DEPS := $(OBJS:.o=.d)

run: CXX_FLAGS += -O0
run: clean bin/main
	@echo "🚀 Running..."
	@./bin/main $(RUN_ARGS)

profile: CXX_FLAGS += -O2
profile: clean bin/main
	@echo "🚀 Profiling..."
	@./bin/main $(RUN_ARGS) profile

bin/main: $(OBJS)
	@mkdir -p $(dir $@)
	@echo "🔗 Linking project..."
	@clang++ $(OBJS) -o $@

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo "⚙️ Compiling $<..."
	@clang++ $(CXX_FLAGS) $(PRE_FLAGS) -c -o $@ $<

clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf build
	@rm -rf bin

-include $(DEPS)
