Introduction
============

This is a Drupal 6.x module to exposing resources RESTfully in a way that
Rails ActiveResource (>= 3.0.0) can consume. It tries very hard to make things
as easy as possible for the developer by providing shortcuts and making
assumptions, but it also tries to be customizable.


Installing
==========

Check out this guy into your `/sites/all/modules directory`, head over to
http://your-site/admin/build/modules and turn it on.


Usage
=====

Call `restfulresource_create( name )` from your module's implementation of
`hook_init()` to create a new resource mapping.


Example
-------

Let's say I want to expose a `books` resource to keep track of my books. I
already have a `library` module that I'm using mostly for this purpose.

In my `library_init` function, I call `restfulresources_create( 'books' )` and
magically I have new routes at http://my-site/books.txt,
http://my-site/books.json and http://my-site/books/%. 
