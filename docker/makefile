# Persistence of storage with in/ and out/ local subdirs.

build-buster:
	docker build --progress plain --build-arg BASE_IMAGE=debian:buster-20240612-slim   -t superpixels:buster-20240612-slim .

build-bullseye:
	docker build --progress plain --build-arg BASE_IMAGE=debian:bullseye-20250520-slim -t superpixels:bullseye-20250520-slim .

build-bookworm:
	docker build --progress plain --build-arg BASE_IMAGE=debian:bookworm-20250520-slim -t superpixels:bookworm-20250520-slim .

build-trixie:
	docker build --progress plain --build-arg BASE_IMAGE=debian:trixie-20250520-slim   -t superpixels:trixie-20250520-slim .

build: build-buster
	docker tag superpixels:buster-20240612-slim superpixels:latest

## Example algorithms section
ifndef s
override s = 100
endif

# Example of help commands to inspect arguments.
help_mss:
	docker run --rm -ti -v `pwd`/in:/in -v `pwd`/out:/out superpixels mss_cli --help

# Example of running the MSS algorithm with a specified scale.
mss: build
	docker run --rm -ti -v `pwd`/in:/in -v `pwd`/out:/out superpixels mss_cli -w -i /in --vis /out -s $(s) --csv /out
