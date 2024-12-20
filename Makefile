CROSS_COMPILER := aarch64-linux-gnu-gcc
COMPILER := gcc

all: create-build-dir kria-app pc-app post-build

create-build-dir:
	@mkdir -p ./build
	@echo "Ensured ./build directory exists.\n"

kria-app: create-build-dir
	@echo "Generating KV260 Application ..\n"
	@${CROSS_COMPILER} -o ./build/kv260-client kv260-client.c

pc-app: create-build-dir
	@echo "Generating HOST-PC Application ..\n"
	@${COMPILER} -o ./build/host-pc-app host-pc-app.c

post-build: ./build/kv260-client ./build/host-pc-app
	@echo "Copying client app to /srv/nfs ..\n"
	@cp $^ /srv/nfs/
