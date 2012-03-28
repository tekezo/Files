all:
	@for d in `find * -type d -maxdepth 0`; do make -C $$d; done

clean:
	@for d in `find * -type d -maxdepth 0`; do make -C $$d clean; done
