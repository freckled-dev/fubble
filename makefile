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
test:
	cd ../fubble_build/meson; meson test # 'matrix test' 'temporary_room test' # 'signalling server test' # 'client room test' # 'session room test' # 'asio_signalling_thread test' # 'websocket test'
