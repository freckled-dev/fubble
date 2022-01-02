BUILD=ninja -C ../fubble_build/meson

.PHONY: all
all: build

.PHONY: build
build:
	cmake --build ../fubble_build

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test:
	meson test -C ../fubble_build/meson

.PHONY: run
run:
	LD_LIBRARY_PATH=../fubble_build/qt/5.15.1/gcc_64/lib \
		../fubble_build/meson/fubble/app/fubble

