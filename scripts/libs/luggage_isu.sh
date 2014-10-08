#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
#
# git clone git@github.com:isubit/luggage_isu.git
# cd luggage_isu
# ./scripts/build_luggage_isu.sh
#

install_luggage_isu_suitcase() {
  # Install Theme - Suitcase
  drush $ALIAS en -y suitcase
  drush $ALIAS vset theme_default suitcase
}

install_luggage_isu_features() {
  # Install luggage_isu features
  drush -v $ALIAS en -y luggage_pubcookie luggage_piwik luggage_people_isu suitcase_config
}