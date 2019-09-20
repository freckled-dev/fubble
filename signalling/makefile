BUILD_DIR = ../build

.PHONY: all
all: build

.PHONY: build
build_armv7: ${BUILD_NATIVE_DIR}/makefile
	$(MAKE) -C ${BUILD_ARMV7_DIR}

${BUILD_ARMV7_DIR}/makefile:
	mkdir -p ${BUILD_ARMV7_DIR}
	ln -rs toolchains/makefile_armv7 ${BUILD_ARMV7_DIR}/makefile


.PHONY: clean
clean:
	$(MAKE) -C ${BUILD_ARMV7_DIR} clean

.PHONY: install
install:
	$(MAKE) -C ${BUILD_ARMV7_DIR} install

.PHONY: distclean
distclean:
	$(MAKE) -C ${BUILD_ARMV7_DIR} distclean



