#!/bin/bash

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
#

# Include luggage functions for building
. $(dirname $0)/libs/luggage.sh

init
install_site
install_luggage_features
finish
