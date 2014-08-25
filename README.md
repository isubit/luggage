Luggage_ISU
=========
[![Build Status](https://travis-ci.org/isubit/luggage_isu.svg?branch=master "Build Status")](http://travis-ci.org/isubit/luggage_isu)

Luggage is a Drupal 7.x platform for building/maintaining academic websites.

  - Add/Merge Luggage to your current Drupal 7.x project.
  - Enable the features you would like to use.
  - Magic.

Luggage is a managed stable release of Drupal 7.x coupled with several features that together make a platform. The platform can be applied at any time:

> The overriding design goal for the Luggage Platform are discrete fully functional features that are independent of the platform and any Drupal theme.
> When the individual features are combined together a full-featured academic platform emerges.
> Luggage is not an install profile nor a distribution. It can be applied to any Drupal 7 project at any time.
> It can also be removed as simply.

Version
----

2.x.x

Technology
-----------

Luggage uses a number of open source projects to work properly:

* [Drupal][] - Drupal is an open source content management platform powering millions of websites and applications.
* [Drush][] - a command-line shell and scripting interface for Drupal
* [Ckeditor][] - HTML text editor designed to simplify web content creation
* [Flexslider2][] - The best responsive slider. Period.
* [Apache Solr][] - search platform.
* [Piwik][] - web analytics platform
* [Pubcookie][] - intra-institutional web authentication

Installation
--------------

The instructions below build a complete luggage with all the features, supporting modules and the suitcase theme.

Assumptions:
* Drush is installed.
* You are located at the root of an existing Git repository.

```
# Connect your git repo to the luggage git repo
git remote add luggage git@github.com:isubit/luggage.git
# Bring the luggage code in
git pull luggage master
# Initialize and retrieve submodules from their own git repos
git submodule init
git submodule update
# The following three commands add your new pristine codebase to your repo --
# the one you created before you added the git remote.
git add -A
git commit -m "picking up luggage"
git push origin master
# Set up a fresh Drupal. If you don't have $DBCREDS defined substitute your
# database username and password in the following command, e.g., root:root
drush si minimal -y --db-url=mysql://$DBCREDS@localhost/myproject --site-name=myproject --account-name=isuitc install_configure_form.update_status_module='array(FALSE,FALSE)'
# Enable luggage Drupal modules
drush en -y luggage_piwik luggage_pubcookie luggage_vars luggage_seo luggage_core luggage_contrib luggage_roles luggage_ckeditor luggage_ui luggage_solr luggage_news luggage_announcements luggage_people suitcase_config
# Reset features to their original configuration settings
drush fra -y
# Clear caches and log in
drush cc all
drush @self uli
```

Troubleshooting
----

Join us on IRC @ ##luggage


License
----

[GPLv2][]


**Open Source | Open Access | Open Mind**

[Drupal]:http://drupal.org/
[Drush]:https://github.com/drush-ops/drush
[Ckeditor]:http://ckeditor.com/
[Flexslider2]:http://flexslider.woothemes.com/
[Apache Solr]:http://lucene.apache.org/solr/
[Piwik]:http://piwik.org/
[Pubcookie]:http://www.pubcookie.org/
[GPLv2]:http://www.gnu.org/licenses/gpl-2.0.html
[Travis]:https://travis-ci.org/isubit/luggage.svg?branch=master
