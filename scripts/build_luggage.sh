#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
# bash <(curl -s https://gist.github.com/hofmockel/8272036/raw/80dcb7040ba7f99c4decefd72a7680c3cac457d8/new-luggage.sh)

ALIAS="@self"

# Some Variables
DIRECTORY=`drush site-alias $ALIAS --component=root`
OS=`uname`
if [ -z "$DBCREDS" ]; then
    printf "DB username: "
    read username
    stty -echo
    printf "DB password: "
    read password
    stty echo
    DBCREDENTIALS=$username:$password
else
    DBCREDENTIALS=$DBCREDS
fi

# Get submodules defined in .gitmodules
git submodule update --init

# Install luggage, all its features and all its dependencies - This should be factored out as a separate function
# Install Drupal 7 using the minimal profile.
drush $ALIAS si minimal -y --db-url=mysql://$DBCREDENTIALS@localhost/luggage --site-name=luggage --account-name=adminn install_configure_form.update_status_module='array(FALSE,FALSE)'

##Install all the Luggage features.
drush -v $ALIAS en -y luggage_announcements luggage_biblio luggage_ckeditor luggage_contrib luggage_core luggage_events luggage_events_solr luggage_indicator luggage_news luggage_placeholder luggage_people luggage_people_solr luggage_projects luggage_resources luggage_resources_solr luggage_roles luggage_roles_solr luggage_seo luggage_solr luggage_ui luggage_vars

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

# Commit all the additions and switch to development branch.
echo "Your luggage is ready."
echo ""

# Provide a one-time login URL
drush $ALIAS uli
