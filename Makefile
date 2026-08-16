CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS ?= -Isrc
SOURCES := src/main.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Checker.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp src/tscc/Project.cpp src/tscc/Config.cpp src/tscc/Compiler.cpp
OBJECTS := $(SOURCES:.cpp=.o)
all: tscc
tscc: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
test-smoke: tscc
	bash tests/smoke.sh
clean:
	rm -f $(OBJECTS) tscc
.PHONY: all test test-smoke test-parser test-edits test-checker test-runtime test-project test-regression test-tsx test-commonjs check-regression-sync clean


test-parser:
	$(CXX) -Isrc $(CXXFLAGS) tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp -o .parser-smoke
	./.parser-smoke
	rm -f .parser-smoke

test-edits:
	$(CXX) -Isrc $(CXXFLAGS) tests/source_edit_invariants.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/SourceEdit.cpp -o .source-edit-test
	./.source-edit-test
	rm -f .source-edit-test

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

test: test-smoke test-parser test-edits test-checker test-runtime test-project test-regression test-tsx test-commonjs
