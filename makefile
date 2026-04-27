CXX = g++

ifeq ($(PROF), true)
CXXPROF += -pg -no-pie
else
CXXPROF += -s
endif

ifeq ($(BUILD), debug)
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -pg -no-pie
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -msse4 -mbmi2 -flto $(CXXPROF) -march=native
endif

ifeq ($(PEXT), true)
CXXFLAGS += -DPEXT
endif

SRC_AI = core/*.cpp ai/*.cpp ai/search/*.cpp
SRC_DUMP = core/*.cpp ai/*.cpp ai/search/*.cpp ai/search/beam/*.cpp

.PHONY: all puyop test clean makedir dump_selfplay wasm

all: puyop

puyop: makedir
	@$(CXX) $(CXXFLAGS) $(SRC_AI) puyop/*.cpp -o bin/puyop/puyop.exe

tuner: makedir
	@$(CXX) $(CXXFLAGS) $(SRC_AI) tuner/*.cpp -o bin/tuner/tuner.exe

dump_selfplay: makedir
	@$(CXX) $(CXXFLAGS) $(SRC_DUMP) tools/dump_selfplay.cpp -o bin/dump_selfplay/dump_selfplay.exe

test: makedir
	@$(CXX) $(CXXFLAGS) $(SRC_AI) test/*.cpp -o bin/test/test.exe

clean: makedir
	@rm -rf bin
	@make makedir

makedir:
	@mkdir -p bin
	@mkdir -p bin/puyop
	@mkdir -p bin/test
	@mkdir -p bin/tuner/data
	@mkdir -p bin/dump_selfplay
	@mkdir -p bin/wasm

.DEFAULT_GOAL := puyop

EMCC = emcc
EMCXXFLAGS = -DUNICODE -DNDEBUG -std=c++20 \
             -msse4.1 -msimd128 \
             -O3 -flto \
             -DEMSCRIPTEN

EMLDFLAGS = -s WASM=1 \
            -s MODULARIZE=1 \
            -s EXPORT_ES6=1 \
            -s ENVIRONMENT=web,worker,node \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s INITIAL_MEMORY=33554432 \
            -s NO_DISABLE_EXCEPTION_CATCHING=1 \
            -s EXPORTED_FUNCTIONS='["_ama_init","_ama_init_preset","_ama_suggest","_ama_diag_field","_ama_diag_weight","_malloc","_free"]' \
            -s EXPORTED_RUNTIME_METHODS='["cwrap","ccall","HEAPU8"]' \
            -s EXPORT_NAME='AmaModule' \
            -fexceptions \
            --embed-file config.json

# form::list は実行時に config.json の "forms" で絞れるので 1 ビルドのみ。
# 訓練 (GTR 専用) は preset "gtr" を ama_init_preset 経由で読み込む。
wasm: makedir
	@$(EMCC) $(EMCXXFLAGS) $(SRC_DUMP) tools/wasm_api.cpp \
		$(EMLDFLAGS) -o bin/wasm/ama.js
