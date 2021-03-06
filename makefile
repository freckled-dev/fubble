BUILD=ninja -C ../fubble_build/meson

.PHONY: all
all: build

.PHONY: build
build:
	meson compile -C ../fubble_build/meson

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test:
	meson test -C ../fubble_build/meson

