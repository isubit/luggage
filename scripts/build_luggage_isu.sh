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

init
install_site

# Install Theme - Suitcase
drush $ALIAS en -y suitcase
drush $ALIAS vset theme_default suitcase

install_luggage_features

# Install luggage_isu features
drush -v $ALIAS en -y luggage_pubcookie luggage_piwik luggage_people_isu suitcase_config

finish
