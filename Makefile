CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
DEPFLAGS ?= -MMD -MP
CPPFLAGS ?= -Isrc
SANITIZER_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
SAN_TARGET := .build/tscc-sanitize
MEMORY_SMOKE := .build/tscc-parser-memory-san
SOURCES := src/main.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp src/tscc/Project.cpp src/tscc/Config.cpp src/tscc/Compiler.cpp
SAN_OBJECTS := $(patsubst %.cpp,.build/san/%.o,$(SOURCES))
OBJECTS := $(SOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)
all: tscc
tscc: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@
test-smoke: tscc
	bash tests/smoke.sh
clean:
	rm -f $(OBJECTS) $(DEPS) tscc
	rm -rf .build
.PHONY: all test test-smoke test-parser test-binder test-types test-edits test-checker test-runtime test-project test-regression test-tsx test-commonjs check-regression-sync test-sanitize memory-safety-smoke clean


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

test: test-smoke test-parser test-binder test-types test-edits test-checker test-runtime test-project test-regression test-tsx test-commonjs

-include $(DEPS)


.build/san/%.o: %.cpp
	mkdir -p "$(dir $@)"
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) -c "$<" -o "$@"

$(SAN_TARGET): $(SAN_OBJECTS)
	mkdir -p .build
	$(CXX) -std=c++17 $(SANITIZER_FLAGS) $(SAN_OBJECTS) -o $(SAN_TARGET)

test-sanitize: $(SAN_TARGET)
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ./$(SAN_TARGET) --version

$(MEMORY_SMOKE): tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp
	mkdir -p .build
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp -o $(MEMORY_SMOKE)

memory-safety-smoke: $(MEMORY_SMOKE)
	mkdir -p .build/memory-safety
	python3 scripts/memory_safety.py --project tscc --mode sanitizer --output .build/memory-safety/checkpoint-0.json --iterations 2 --command './$(MEMORY_SMOKE)'
