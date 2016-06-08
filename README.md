Luggage
=========
[![Build Status](https://travis-ci.org/isubit/luggage.svg?branch=master "Build Status")](http://travis-ci.org/isubit/luggage)

Introduction
-------------------
Luggage is a Drupal 7.x platform for building/maintaining academic websites. The platform is built on Drupal and consists of features that come with configuration. The goal is to quickly leverage common use cases so that web developers can be empowered to get sites up and running quickly but maintain the ability to customize features or add additional features. [Contributions](http://luggagedocs.info/contributing) are welcome!

Some of the features found in Luggage are:

* Friendly, intuitive URLs
* WYSIWYG CKEditor configured
* Content types and views for Events, news, people profiles, projects/grants, resources, and more...
* Apache Solr Search
* All content is tagged and categorized
* Great for sites with lots of content. Prefers a search-based architecture rather than a hierarchal menu architecture
* Theme independent

The overriding design goal for the Luggage Platform are discrete fully functional features that are independent of any Drupal theme. When the individual features are combined together a full-featured academic platform emerges. Luggage is not an install profile nor a distribution.

**Other forks worth looking at:**

* [Luggage ISU](http://github.com/isubit/luggage_isu) - Fork with theme and configuration for Iowa State University.
* Your Fork - Since Luggage is generic and has no theme, you may want to create your own fork for your organization.

More about Luggage can be found at http://luggagedocs.info

Requirements
-----------------
* [Drush](http://www.drush.org/) 5.9
* Git 1.8.5.2 or later
* [Drupal 7.x Requirements](https://www.drupal.org/requirements)
* Intermediate knowledge of Drupal site building and administration
* [Apache Solr](http://lucene.apache.org/solr/)
* Windows environment unsupported

Installation
--------------
* In your web server's document root: `git clone git@github.com:isubit/luggage.git` 
* `cd luggage`
* `bash scripts/build_luggage.sh` This script makes some assumptions such as using a localhost database. It may be helpful to define bash environment variables for `$APACHEUSER` and `$DBCREDS`.
* If the installation script is successful, it will provide you with a one-time log in link for the admin user.

More verbose instructions can be found at http://luggagedocs.info/installing-luggage-scratch


Versions
----
Development of Luggage takes place on the 'development' branch in Github. Pull requests should be made against the 'development' branch.

Releases take place on the 'master' branch. 

The 2.x.x branches and lower of Luggage are no longer supported.

Contributing
----

Refer to the [Luggage git architecture](http://www.biology-it.iastate.edu/luggage_doc/comprehensive-code-flow-management)

Refer to the [contribution documentation](http://www.biology-it.iastate.edu/luggage_doc/luggage-development)

Troubleshooting
----
Read/Search http://luggagedocs.info

Join us on IRC FreeNode @ #luggage
* Need help setting up IRC? https://www.drupal.org/irc/setting-up

Travis-CI - https://travis-ci.org/isubit/luggage
* Reports into IRC FreeNode @  ##luggage

License
----
[GPLv2](http://www.gnu.org/licenses/gpl-2.0.html)


**Open Source | Open Access | Open Mind**
