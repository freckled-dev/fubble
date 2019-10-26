BUILD=time -f 'finished after %E' ninja -C ../fubble_build/meson

.PHONY: all
all:
	${BUILD}

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test:
	${BUILD} test
