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


# Ensure we are at the root of a Drupal site.
DRUSHVERSION=$(drush --version --pipe)

if [[ $DRUSHVERSION == "5."* ]]; then
  DRUPALROOT=$(drush site-alias @self --component=root)
else
  # Drush 6, 7, 8 all support the following syntax.
  DRUPALROOT=$(drush site-alias @self --format=csv --fields=root --field-labels=0)
fi

DIRECTORY=$(pwd)

# Get name of directory we are currently in
# to use as the database name and such
# http://stackoverflow.com/questions/1371261/get-current-directory-name-without-full-path-in-bash-script?answertab=active#tab-top
BASENAME=${PWD##*/}

init() {
    if [ "$DIRECTORY" != "$DRUPALROOT" ]; then
        echo "Please run $0 from the root of a Drupal site." && exit 1
    fi

    echo "Proceeding with site name -> " $BASENAME

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
    
    [ -z "$APACHEUSER" ] && APACHEUSER=apache

    # Get submodules defined in .gitmodules
    git submodule update --init --force
}

install_site() {
    # Install luggage, all its features and all its dependencies - This should be factored out as a separate function
    # Install Drupal 7 using the minimal profile.
    drush si minimal -y --db-url=mysql://$DBCREDENTIALS@localhost/$BASENAME --site-name=$BASENAME --account-name=adminn install_configure_form.update_status_module='array(FALSE,FALSE)'
}

install_luggage_features() {
    # Install only the Luggage features you want.
    #drush -v en -y luggage_announcements luggage_biblio luggage_ckeditor luggage_contrib luggage_core luggage_events luggage_events_solr luggage_indicator luggage_news luggage_news_solr luggage_placeholder luggage_people luggage_people_expertise luggage_people_solr luggage_projects luggage_resources luggage_resources_solr luggage_roles luggage_roles_solr luggage_seo luggage_solr luggage_ui luggage_vars

    # Install all the Luggage features.
    drush en -y luggage_*
    
    if [[ $DRUSHVERSION == "7."* || $DRUSHVERSION == "8."* ]]; then
      # Drush 8 won't enable a module if it includes a dependency that isn't listed on d.o.
      # Running again to actually enable the features instead of just getting the dependencies.
      # See LUGG-680
      echo "Running drush en again"
      drush en -y luggage_*
    fi
}

finish() {
    # Revert all the Luggage features.
    drush fra -y

    # Clear all Drupal cache.
    drush cc all
    drush cron-run all --cli all > /dev/null 2>&1

    # New files directory
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
    drush uli
}
