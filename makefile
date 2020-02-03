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
	cd ../fubble_build/meson; meson test # 'client room test' # 'signalling server test' # 'asio_signalling_thread test' # 'websocket test'  
