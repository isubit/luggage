Luggage
=========
[![Build Status](https://travis-ci.org/isubit/luggage.svg?branch=master "Build Status")](http://travis-ci.org/isubit/luggage)

Luggage is a Drupal 7.x platform for building/maintaining academic websites.

  - Build Luggage from script or Add/Merge Luggage to your current Drupal 7.x project.
  - Enable the features you would like to use.
  - Magic.

Luggage is a managed stable release of Drupal 7.x coupled with several features that together make a platform. The platform can be applied at any time:

> The overriding design goal for the Luggage Platform are discrete fully functional features that are independent of the platform and any Drupal theme.
> When the individual features are combined together a full-featured academic platform emerges.
> Luggage is not an install profile nor a distribution. It can be applied to any Drupal 7 project at any time.
> It can also be removed as simply.

Is Luggage Right For You?
-------------------------
An Iowa State University flavor of Luggage, [Luggage ISU][] is available for those intending to build and deploy at ISU.

If this is not you, then Luggage is right for you!

Version
----
Development of Luggage takes place on the 'development' branch in Github. Pull requests should be made against the 'development' branch.

Releases take place on the 'master' branch. 

The 2.x.x branch of Luggage is being maintained for security fixes only.

Upgrading from Luggage 2.x.x to 3.x.x
-------------------------------------
1. Use git to upgrade your code
2. Update your git submodules

Technology
-----------

Luggage uses a number of open source projects to work properly:

* [Drupal][] - Drupal is an open source content management platform powering millions of websites and applications.
* [Drush][] - a command-line shell and scripting interface for Drupal
* [Ckeditor][] - HTML text editor designed to simplify web content creation
* [Flexslider2][] - The best responsive slider. Period.
* [Apache Solr][] - search platform.

Installation
--------------

Below is the brief run-down on how to install Luggage. The full installation documentation can be found [here](http://www.biology-it.iastate.edu/luggage_doc/installing-luggage-scratch#).

#####Assumptions:
* Drush is installed
* Git is installed

Clone this repo's latest release into the root directory that your web server points to.
Releases: https://github.com/isubit/luggage/releases


Run the build script found within the *scripts* directory:
```
source ./scripts/build_luggage.sh
```

The last line of output from the script, if successful, will be a root user one-time login link. Copy and append everything from "/user" and on, to the end of the luggage installation url in a web browser of your choice. Typically, the final link will look something like this:
``` 
http://localhost/luggage/user/reset/1/1409061179/KjbHsr6O7FRaz-__WShbgWuPwKHKrXHy6QGV_AQu02E/login
```

You are now logged in as the root user, allowing you to develop using the power of Luggage! Enjoy!

Contributing
----

Refer to the [Luggage git architecture](http://www.biology-it.iastate.edu/luggage_doc/comprehensive-code-flow-management)

Refer to the [contribution documentation](http://www.biology-it.iastate.edu/luggage_doc/luggage-development)

Troubleshooting
----
Read/Search [Luggage Documentation][]

Join us on IRC FreeNode @ #luggage
* Need help setting up IRC? https://www.drupal.org/irc/setting-up

Travis-CI - https://travis-ci.org/isubit/luggage
* Reports into IRC FreeNode @  ##luggage

License
----

[GPLv2][]


**Open Source | Open Access | Open Mind**

[Drupal]:http://drupal.org/
[Drush]:https://github.com/drush-ops/drush
[Ckeditor]:http://ckeditor.com/
[Flexslider2]:http://flexslider.woothemes.com/
[Apache Solr]:http://lucene.apache.org/solr/
[GPLv2]:http://www.gnu.org/licenses/gpl-2.0.html
[Travis]:https://travis-ci.org/isubit/luggage.svg?branch=master
[Luggage ISU]:https://github.com/isubit/luggage_isu
[Luggage Ducumentation]:http://www.biology-it.iastate.edu/luggage_doc/

