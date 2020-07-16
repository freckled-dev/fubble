BUILD=time -f 'finished after %E' ninja -C ../fubble_build/meson

.PHONY: all
all: build

.PHONY: build
build:
	cd ../fubble_build/meson; meson compile

.PHONY: clean
clean:
	${BUILD} clean

.PHONY: install
install:
	${BUILD} install

.PHONY: test
test:
	cd ../fubble_build/meson; meson test # "webrtc_google_peer test"
	# ../fubble_build/meson/client/app/fubble
