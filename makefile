BUILD=ninja -C ../fubble_build/cmake

.PHONY: all
all: build

.PHONY: build
build:
	cmake --build ../fubble_build/cmake

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test: build
	cmake --build ../fubble_build/cmake --target test

.PHONY: run
run:
	LD_LIBRARY_PATH=../fubble_build/qt/5.15.1/gcc_64/lib \
		../fubble_build/meson/fubble/app/fubble

