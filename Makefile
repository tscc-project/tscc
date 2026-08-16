CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
DEPFLAGS ?= -MMD -MP
CPPFLAGS ?= -Isrc
SOURCES := src/main.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Semantic.cpp src/tscc/Binder.cpp src/tscc/Type.cpp src/tscc/Checker.cpp src/tscc/SourceEdit.cpp src/tscc/Transpiler.cpp src/tscc/Project.cpp src/tscc/Config.cpp src/tscc/Compiler.cpp
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
.PHONY: all test test-smoke test-parser test-binder test-types test-edits test-checker test-runtime test-project test-regression test-tsx test-commonjs check-regression-sync clean


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
