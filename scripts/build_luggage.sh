#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
# - Luggage has been merged - load-luggage https://gist.github.com/hofmockel/8270938
# - Assumes your database credentials to be in $DBCREDS environment variable
# bash <(curl -s https://gist.github.com/hofmockel/8272036/raw/80dcb7040ba7f99c4decefd72a7680c3cac457d8/new-luggage.sh)

# FORK=$1

ALIAS='@self'

# Some Variables
DIRECTORY=`drush site-alias $ALIAS --component=root`
OS=`uname`

# If target directory already exists, move it to $DIRECTORY.bak
# if [ -d "$DIRECTORY" ]; then
#   # You would think the following would work, but mv refuses to overwrite.
#   # mv -f $DIRECTORY ${DIRECTORY}.bak
#   # So we use rsync and then remove the original.
#   if [ "$OS" = 'Darwin' ]; then
#     echo "Your Macintosh account password may be required"
#   fi
#   sudo rsync -a --delete $DIRECTORY ${DIRECTORY}.bak
#   sudo chown $USER ${DIRECTORY}.bak
#   sudo rm -rf $DIRECTORY
# fi
git submodule update --init

# creates new local repo called luggage-dev
# recursively clone the submodules as well
# git clone $FORK luggage-dev --recursive

# loads luggage - This should be factored out as a separate function
# cd luggage-dev

# Install luggage, all its features and all its dependencies - This should be factored out as a separate function
# Install Drupal 7 using the minimal profile.
drush $ALIAS si minimal -y --db-url=mysql://$DBCREDS@localhost/luggage --site-name=luggage --account-name=adminn install_configure_form.update_status_module='array(FALSE,FALSE)'

# Install theme
# if [ -d "sites/all/themes/suitcase" ]; then
#   # Install Omega and Suitcase
#   drush @luggage.local en -y suitcase
#   drush @luggage.local vset theme_default suitcase
# fi

# if [ -e "build.sh" ]; then
#   echo "Using build script mixins"
#   source build.sh
# fi

##Install all the Luggage features.
drush -v $ALIAS en -y luggage_announcements luggage_biblio luggage_ckeditor luggage_contrib luggage_core luggage_events luggage_events_solr luggage_indicator luggage_news luggage_placeholder luggage_people luggage_people_isu luggage_people_solr luggage_piwik luggage_projects luggage_pubcookie luggage_resources luggage_resources_solr luggage_roles luggage_roles_solr luggage_seo luggage_solr luggage_ui luggage_vars

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

# Ensure presence of development modules and absence of piwik
if [[ "$ISU_ENVIRONMENT" == 'DEVELOPMENT' || "$ISU_ENVIRONMENT" == 'TESTING' ]]; then
  drush $ALIAS dl security_review devel hacked devel_themer simplehtmldom-7.x-1.12 get_form_id
  drush $ALIAS en -y security_review hacked
fi

# generate content to look at
#VOCAB=$(drush @luggage.local genv --kill 3 2>&1 | tail -3 | head -n1) && drush @luggage.local gent --kill $VOCAB 20 && drush @luggage.local --kill genu 50 && drush @luggage.local --kill genc 200 && drush @luggage.local --kill genm 5

# Commit all the additions and switch to development branch.
echo "Your luggage is ready."
echo ""
#git checkout -b development

# Provide a one-time login URL
drush $ALIAS uli
# # Open site if running from a Mac.
# if [ "$OS" = 'Darwin' ]; then
#   open `drush $ALIAS uli`
# else
#   drush $ALIAS uli
# fi