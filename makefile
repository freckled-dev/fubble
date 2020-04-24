BUILD=time -f 'finished after %E' ninja -C ../fubble_build/meson

.PHONY: all
all: build

.PHONY: build
build:
	${BUILD}

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test: build
	cd ../fubble_build/meson; meson test --test-args='--gtest_shuffle'
	# ../fubble_build/meson/client/app/client_gui -style Material
