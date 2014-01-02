Luggage
=========

Luggage is a Drupal 7.x platform for building/maintaining academic websites.

  - Add/Merge Luggage to your current Drupal 7.x project.
  - Enable the features you would like to use.
  - Magic

Luggage is a managed stable release of Drupal 7.x coupled with several features that together make a platform. The platform can be applied at anytime:

> The overriding design goal for the Luggage Platform are discrete fully functional features that are independent of the platform and any Drupal theme.
> When the individual features are combined together a full featured academic platform emerges.
> Luggage is not an install profile nor a distribution. It can be applied to any Drupal 7 project at any time.
> It can also be removed as simply.

Version
----

1.0.0

Tech
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

Assumptions:
* Drush is installed.
* You are located at the root of an existing Git repository.

```
git remote add luggage git@bitbucket.org:isuitc/luggage.git
git pull luggage master
git submodule init
git submodule update
git add *
git commit -m "luggage"
git push origin master
drush si minimal -y --db-url=mysql://root:root@localhost/myproject --site-name=myproject --account-name=isuitc install_configure_form.update_status_module='array(FALSE,FALSE)'
drush en -y piwik_config pubcookie_config set_corevars global_seo global_coreenabled global_otherenabled global_roles ckeditor_config enable_ui_modules solr_config news announcements people suitcase_config
drush fra -y
drush cc all
drush @self uli
```

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