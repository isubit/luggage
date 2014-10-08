#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory of a recently cloned
#   luggage_isu. Example:
#
# git clone git@github.com:isubit/luggage_isu.git
# cd luggage_isu
# ./scripts/build_luggage_isu.sh
#

# Inlcude luggage functions for building
. $(dirname $0)/libs/luggage.sh
# Inlcude luggage_isu functions for building
. $(dirname $0)/libs/luggage_isu.sh

init
install_site

install_luggage_isu_suitcase

install_luggage_features

install_luggage_isu_features

finish
