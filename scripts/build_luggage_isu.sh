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

ALIAS="@self"

# Ensure we are at the root of a Drupal site.
DRUPALROOT=$(drush site-alias $ALIAS --component=root)
DIRECTORY=$(pwd)

if [ "$DIRECTORY" != "$DRUPALROOT" ]; then
  echo "Please run $0 from the root of a Drupal site." && exit 1
fi

if [ -z "$DBCREDS" ]; then
    printf "DB username: "
    read username
    stty -echo
    printf "DB password: "
    read password
    stty echo
    DBCREDS=$username:$password
else
    DBCREDS=$DBCREDS
fi

# Get submodules defined in .gitmodules
git submodule update --init

# Install luggage, all its features and all its dependencies.
# Install Drupal 7 using the minimal profile.
drush $ALIAS si minimal -y --db-url=mysql://$DBCREDS@localhost/luggage_isu --site-name=luggage_isu --account-name=adminn install_configure_form.update_status_module='array(FALSE,FALSE)'

# Install Theme - Suitcase
drush $ALIAS en -y suitcase
drush $ALIAS vset theme_default suitcase

##Install all the Luggage features.
drush -v $ALIAS en -y luggage_announcements luggage_biblio luggage_ckeditor luggage_contrib luggage_core luggage_events luggage_events_solr luggage_indicator luggage_news luggage_placeholder luggage_people luggage_people_solr luggage_projects luggage_resources luggage_resources_solr luggage_roles luggage_roles_solr luggage_seo luggage_solr luggage_ui luggage_vars

# Install suitcase features
drush -v $ALIAS en -y luggage_pubcookie luggage_piwik luggage_people_isu suitcase_config

# Revert all the Luggage features.
drush $ALIAS fra -y

# Clear all Drupal cache.
drush $ALIAS cc all
drush $ALIAS cron-run all --cli all > /dev/null 2>&1

# new files directory
mkdir -p $DIRECTORY/files
sudo chown -R $APACHEUSER $DIRECTORY/files
sudo chmod -R g+w $DIRECTORY/files

# Add a conditional include for settings.env.inc
echo "
// Environment specific settings.
\$environment_settings = '/var/www/env/settings.env.inc';

if (file_exists(\$environment_settings)) {
require(\$environment_settings);
}" | sudo tee -a $DIRECTORY/sites/default/settings.php

# Check for existence of /var/www/env/settings.env.inc
if [ ! -e /var/www/env/settings.env.inc ]; then
  echo "Warning: /var/www/env/settings.env.inc does not exist"
fi

# Commit all the additions and switch to development branch.
echo "Your luggage is ready."
echo ""

# Provide a one-time login URL
drush $ALIAS uli
