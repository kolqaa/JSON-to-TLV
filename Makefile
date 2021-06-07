BUILD_DIR=build

all:
	cmake -B${BUILD_DIR} -H.
	cmake --build ${BUILD_DIR} --target all

clean:
	rm -rf ${BUILD_DIR}
