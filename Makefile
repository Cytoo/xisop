all:
	for tme in $(wildcard tme*/); do \
		echo $$tme; \
		make -C $$tme; \
	done
