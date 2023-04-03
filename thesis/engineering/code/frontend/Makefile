.PHONY: devel clean push

dev:
	yarn dev

clean:
	rm -r node_modules dist .cache

push:
	yarn build
	rm -r dist/*.map
