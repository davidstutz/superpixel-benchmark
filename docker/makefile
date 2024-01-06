# Persistence of storage with in/ and out/ local subdirs.
# Presumes .bmp images are ready in `in/`. See `convert` if needed.
run: build
	mkdir -p in/ raw/ out/
	docker run --rm -ti \
		-v `pwd`/in:/app/in \
		-v `pwd`/out:/app/out \
		-v `pwd`/raw:/app/raw:ro \
		superpixels bash

build:
	docker build -t superpixels .

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

