all:
	mkdir -p build \
		&& cd build \
		&& cmake .. \
		&& make

clean:
	rm -rf build

run:
	bash run.bash

update_vendor:
	rm -rf vendor
	cget install -f cget-requirements.txt --prefix vendor
