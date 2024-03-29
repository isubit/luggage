Luggage
=========
[![Build Status](https://travis-ci.org/isubit/luggage.svg?branch=master "Build Status")](http://travis-ci.org/isubit/luggage)

Introduction
-------------------
Luggage is a platform for building/maintaining academic websites. The platform is built on Drupal 7 and consists of features that come with configuration. The goal is to empower web developers to quickly leverage common use cases so that sites can be up and running quickly but maintain the ability to customize features or add additional features. 

Some of the features found in Luggage are:

* Friendly, intuitive URLs
* WYSIWYG CKEditor configured
* Content types and views for events, news, people profiles, projects/grants, resources, and more...
* Faceted search provided by Apache Solr
* All content is tagged and categorized
* Great for sites with lots of content. Prefers a search-based architecture rather than a hierarchal menu architecture
* Theme-independent

For a quick overview, see [Luggage at a Glance](http://www.biology-it.iastate.edu/luggage_doc/luggage-glance).

The overriding design goal for the Luggage platform is to provide discrete features that are independent of any Drupal theme. When the individual features are combined together a full-featured academic platform emerges.

**Other forks worth looking at:**

* [Luggage ISU](http://github.com/isubit/luggage_isu) - Downstream fork with theme and configuration for Iowa State University.
* Your Fork - Since Luggage is generic and has no theme, you may want to create your own fork for your organization.

Requirements
-----------------
* [Drush](http://www.drush.org/) 5.9 or later
* Git 1.8.5.2 or later
* [Drupal 7.x Requirements](https://www.drupal.org/requirements)
* Intermediate knowledge of Drupal site building and administration
* [Apache Solr](http://lucene.apache.org/solr/) (optional)
* Windows environment unsupported

Installation
--------------
* In your web server's document root: `git clone git@github.com:isubit/luggage.git` 
* `cd luggage`
* `bash scripts/build_luggage.sh` It may be helpful to define bash environment variables for `$APACHEUSER` and `$DBCREDS`. It will assume your database is at localhost unless you have defined a different host in `$DBHOST`.
* If the installation script is successful, it will provide you with a one-time log in link for the admin user.

Versions
----
Development of Luggage takes place on the 'development' branch in Github. Pull requests should be made against the 'development' branch.

Releases take place on the 'master' branch. 

The 2.x.x branches and lower of Luggage are no longer supported.

Contributing
----

Refer to the [Luggage git architecture](http://www.biology-it.iastate.edu/luggage_doc/comprehensive-code-flow-management). Pull requests against the development branch are encouraged.

Refer to the [contribution documentation](http://www.biology-it.iastate.edu/luggage_doc/luggage-development)

Thanks
----
Thanks to the following companies for providing sponsorship through services and products used to build and improve Luggage:
- [BrowserStack](https://www.browserstack.com) for interface testing across desktop and mobile browsers.
- [JetBrains](https://www.jetbrains.com/phpstorm/) for use of the PHPStorm IDE for development.
- [GitHub](https://github.com) for collaboration and code management tools.
- [Travis-CI](https://travis-ci.org/) for continuous automated testing.

License
----
[GPLv2](http://www.gnu.org/licenses/gpl-2.0.html)


**Open Source | Open Access | Open Mind**
