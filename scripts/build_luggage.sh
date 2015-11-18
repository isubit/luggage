#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
#
# Typical Usage:
# =============
# ./scripts/build_luggage.sh
# Or optionally specify a site alias and server
# ./scripts/build_luggage.sh @compsci.live
#

# Include luggage functions for building
. $(dirname $0)/libs/luggage.sh

init $(get_site_identifier $1)
install_site
install_luggage_features
finish
