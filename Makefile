SOURCE_ROOT := ${shell pwd}
BUILD_DIR  := ${SOURCE_ROOT}/build

all:
	cmake -B${BUILD_DIR} -H.
	cmake --build ${BUILD_DIR} --target all

clean:
	rm -rf ${BUILD_DIR}
