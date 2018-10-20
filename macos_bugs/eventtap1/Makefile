all:
	mkdir -p build_xcode \
		&& cd build_xcode \
		&& cmake -G Xcode .. \
		&& xcodebuild -configuration Release SYMROOT="$(CURDIR)/build_xcode/build"

run:
	./build_xcode/build/Release/EventTapTest.app/Contents/MacOS/EventTapTest
