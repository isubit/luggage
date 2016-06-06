# Makefile for tugboat.qa

tugboat-init:
	curl -O https://code.ent.iastate.edu/tugboat/tugboat-init.sh
	bash -x tugboat-init.sh

tugboat-update:
	curl -O https://code.ent.iastate.edu/tugboat/tugboat-update.sh
	bash -x tugboat-update.sh

tugboat-build:
	curl -O https://code.ent.iastate.edu/tugboat/tugboat-build.sh
	bash -x tugboat-build.sh
