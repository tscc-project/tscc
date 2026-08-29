CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
DEPFLAGS ?= -MMD -MP
CPPFLAGS ?= -Isrc
SANITIZER_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
SAN_TARGET := .build/tscc-sanitize
MEMORY_SMOKE := .build/tscc-parser-memory-san
SOURCES := src/main.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/CompilationUnit.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp src/tscc/Project.cpp src/tscc/Config.cpp src/tscc/Compiler.cpp
SAN_OBJECTS := $(patsubst %.cpp,.build/san/%.o,$(SOURCES))
SAN_DEPS := $(SAN_OBJECTS:.o=.d)
OBJECTS := $(SOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)
all: tscc
tscc: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $@
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@
test-smoke: tscc
	bash tests/smoke.sh
clean:
	rm -f $(OBJECTS) $(DEPS) tscc
	rm -rf .build
.PHONY: all test test-core test-smoke test-parser test-binder test-types test-edits test-checker test-compilation-unit test-program-graph test-runtime test-project test-regression test-tsx test-commonjs test-product-boundary test-feature-matrix check-regression-sync test-sanitize memory-safety-smoke clean


test-parser:
	$(CXX) -Isrc $(CXXFLAGS) tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp -o .parser-smoke
	./.parser-smoke
	rm -f .parser-smoke

test-edits:
	$(CXX) -Isrc $(CXXFLAGS) tests/source_edit_invariants.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/SourceEdit.cpp -o .source-edit-test
	./.source-edit-test
	rm -f .source-edit-test

test-binder:
	$(CXX) -Isrc $(CXXFLAGS) tests/binder_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp -o .binder-smoke
	./.binder-smoke
	rm -f .binder-smoke

test-types:
	$(CXX) -Isrc $(CXXFLAGS) tests/type_model_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp -o .type-model-smoke
	./.type-model-smoke
	rm -f .type-model-smoke

test-checker: tscc
	bash tests/typecheck_variables.sh

test-compilation-unit:
	$(CXX) -Isrc $(CXXFLAGS) tests/compilation_unit_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/CompilationUnit.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp -o .compilation-unit-smoke
	./.compilation-unit-smoke
	rm -f .compilation-unit-smoke

test-program-graph:
	$(CXX) -Isrc $(CXXFLAGS) tests/program_graph_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/CompilationUnit.cpp src/tscc/Project.cpp -o .program-graph-smoke
	./.program-graph-smoke
	rm -f .program-graph-smoke

test-runtime: tscc
	bash tests/runtime_features.sh
test-project: tscc
	bash tests/project_modules.sh

test-regression: tscc
	python3 regression/run.py --tscc "$(CURDIR)/tscc"

check-regression-sync:
	bash tools/check_regression_sync.sh

test-tsx: tscc
	bash tests/tsx_preserve.sh

test-commonjs: tscc
	bash tests/commonjs_modules.sh

test-product-boundary:
	bash tools/check_product_boundary.sh

test-feature-matrix:
	python3 tools/check_feature_matrix.py

test: test-product-boundary test-feature-matrix test-smoke test-parser test-binder test-types test-edits test-checker test-compilation-unit test-program-graph test-runtime test-project test-regression test-tsx test-commonjs

# Compiler-core gates that do not require a native node/tsc round-trip. CI
# runs this on Windows (msys2): the node/tsc differential gates (runtime,
# project, tsx, commonjs) are fully exercised on Linux and macOS, and the
# external regression corpus runs on Windows as a native-Python step.
test-core: test-product-boundary test-feature-matrix test-smoke test-parser test-binder test-types test-edits test-checker test-compilation-unit test-program-graph

-include $(DEPS)


.build/san/%.o: %.cpp
	mkdir -p "$(dir $@)"
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) -MMD -MP -c "$<" -o "$@"

$(SAN_TARGET): $(SAN_OBJECTS)
	mkdir -p .build
	$(CXX) -std=c++17 $(SANITIZER_FLAGS) $(SAN_OBJECTS) -o $(SAN_TARGET)

test-sanitize: $(SAN_TARGET)
	ASAN_OPTIONS="$${ASAN_OPTIONS:-detect_leaks=1:halt_on_error=1}" UBSAN_OPTIONS=halt_on_error=1 ./$(SAN_TARGET) --version

-include $(SAN_DEPS)

$(MEMORY_SMOKE): tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp
	mkdir -p .build
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp -o $(MEMORY_SMOKE)

memory-safety-smoke: $(MEMORY_SMOKE)
	mkdir -p .build/memory-safety
	python3 scripts/memory_safety.py --project tscc --mode sanitizer --output .build/memory-safety/checkpoint-0.json --iterations 2 --command './$(MEMORY_SMOKE)'

MEMORY_LIFETIME := .build/tscc-memory-lifetime
MEMORY_LIFETIME_SAN := .build/tscc-memory-lifetime-san
MEMORY_LIFETIME_SOURCES := tests/memory_lifetime.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/CompilationUnit.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp

$(MEMORY_LIFETIME):
	mkdir -p .build
	$(CXX) $(CPPFLAGS) -std=c++17 -O2 -Wall -Wextra -pedantic $(MEMORY_LIFETIME_SOURCES) -o $(MEMORY_LIFETIME)

$(MEMORY_LIFETIME_SAN):
	mkdir -p .build
	$(CXX) $(CPPFLAGS) -std=c++17 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wall -Wextra -pedantic $(MEMORY_LIFETIME_SOURCES) -o $(MEMORY_LIFETIME_SAN)

memory-safety-checkpoint-5: $(MEMORY_LIFETIME_SAN) $(SAN_TARGET)
	mkdir -p .build/memory-safety
	env -u LD_PRELOAD python3 scripts/memory_safety.py --project tscc --mode sanitizer --output .build/memory-safety/checkpoint-5-lifetime.json --iterations 1 --command './$(MEMORY_LIFETIME_SAN) 80'
	env -u LD_PRELOAD python3 scripts/checkpoint5_project_lifetime.py --tscc "$(CURDIR)/$(SAN_TARGET)" --rounds 8 --files 120 --output .build/memory-safety/checkpoint-5-project-sanitizer.json

memory-safety-checkpoint-5-rss: $(MEMORY_LIFETIME) tscc
	mkdir -p .build/memory-safety
	python3 scripts/memory_safety.py --project tscc --mode rss --output .build/memory-safety/checkpoint-5-rss.json --warmup-iterations 5 --iterations 40 --command './$(MEMORY_LIFETIME) 20'
	python3 scripts/checkpoint5_project_lifetime.py --tscc "$(CURDIR)/tscc" --rounds 20 --files 120 --output .build/memory-safety/checkpoint-5-project.json

valgrind-memory-safety-checkpoint-5: $(MEMORY_LIFETIME)
	mkdir -p .build/memory-safety
	python3 scripts/memory_safety.py --project tscc --mode valgrind --output .build/memory-safety/checkpoint-5-valgrind.json --iterations 1 --command './$(MEMORY_LIFETIME) 40'

.PHONY: memory-safety-checkpoint-5 memory-safety-checkpoint-5-rss valgrind-memory-safety-checkpoint-5
