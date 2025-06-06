# Persistence of storage with in/ and out/ local subdirs.
# Presumes .bmp images are ready in `in/`. See `convert` if needed.
run: build
	mkdir -p in/ raw/ out/
	docker run --rm -ti \
		-v `pwd`/in:/app/in \
		-v `pwd`/out:/app/out \
		-v `pwd`/raw:/app/raw:ro \
		superpixels bash

build-buster:
	docker build --build-arg BASE_IMAGE=debian:buster-20240612-slim   -t superpixels:buster-20240612-slim .

build-bullseye:
	docker build --build-arg BASE_IMAGE=debian:bullseye-20250520-slim -t superpixels:bullseye-20250520-slim .

build-bookworm:
	docker build --build-arg BASE_IMAGE=debian:bookworm-20250520-slim -t superpixels:bookworm-20250520-slim .

build-trixie:
	docker build --build-arg BASE_IMAGE=debian:trixie-20250520-slim   -t superpixels:trixie-20250520-slim .

build: build-buster
	docker tag superpixels:buster-20240612-slim superpixels:latest
	@echo "Superpixels container with 'latest' tag built successfully."

setup-buildx:
	docker buildx create --name multiarch

# Images need to be in .bmp format, so `imagemagick` is bundled in the container for convenience
# Put images in ./raw/
convert:
	mkdir -p in/
	rm -f in/* || true
	docker run --rm -ti -v `pwd`/in:/app/in -v `pwd`/raw:/app/raw:ro superpixels convert raw/*.png in/*.bmp

# Example of animating frames within the container - helpful for batch processing.
extract:
	mkdir -p in/
	rm -f in/* || true
	docker run --rm -ti \
		-v `pwd`/in:/app/in \
		-v `pwd`/raw:/app/raw:ro \
		ffmpeg -i /raw/video.mp4 \
		-vf fps=24 \
		in/frame%04d.bmp

animate:
	docker run --rm -ti \
		-v `pwd`/out:/app/out \
		ffmpeg -i out/frame%04d.png \
		-c:v libx264 \
		-vf "fps=24,format=yuv420p,pad=ceil(iw/2)*2:ceil(ih/2)*2" \
		out/out.mp4

clean_out:
	mkdir -p out/
	rm -f out/* || true

## Example algorithms section
ifndef s
override s = 100
endif

# Example of help commands to inspect arguments.
help_mss:
	docker run --rm -ti -v `pwd`/in:/in -v `pwd`/out:/out superpixels bash -c "/app/super/bin/mss_cli --help"

mss: clean_out
	docker run --rm -ti -v `pwd`/in:/in -v `pwd`/out:/out superpixels bash -c "/app/super/bin/mss_cli -w -i /in --vis /out -s $(s) --csv /out"

