CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS ?= -Isrc
SOURCES := src/main.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp src/tscc/Transpiler.cpp src/tscc/Project.cpp src/tscc/Config.cpp src/tscc/Compiler.cpp
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
.PHONY: all test test-smoke test-parser test-runtime test-project test-regression test-tsx test-commonjs clean


test-parser:
	$(CXX) -Isrc $(CXXFLAGS) tests/parser_smoke.cpp src/tscc/Diagnostic.cpp src/tscc/Source.cpp src/tscc/Lexer.cpp src/tscc/Parser.cpp -o .parser-smoke
	./.parser-smoke
	rm -f .parser-smoke

test-runtime: tscc
	bash tests/runtime_features.sh
test-project: tscc
	bash tests/project_modules.sh

test-regression: tscc
	python3 regression/run.py --tscc "$(CURDIR)/tscc"

test-tsx: tscc
	bash tests/tsx_preserve.sh

test-commonjs: tscc
	bash tests/commonjs_modules.sh

test: test-smoke test-parser test-runtime test-project test-regression test-tsx test-commonjs
