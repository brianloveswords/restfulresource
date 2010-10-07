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
magically I have new routes at `http://my-site/books.txt`,
`http://my-site/books.json` and `http://my-site/books/%`. Unless I'm writing
an API server, I probably want to move those routes to api/books. To do this,
call restfulresources_create with the optional prefix parameter like so:
`restfulresources_create( 'books', 'api/1.0' )`. This will make routes like
`http://my-site/api/1.0/books/%`

In addtion to the free routes, `restfulresources_create` sets up action
routing for the Create Read Update Delete actions, as well as HEAD method
action for checking existence.

In this case I want to add a custom 'summary' action that responds to GET
requests and returns a short summary of a book.

    $resource = restfulresources_create( 'books', 'api/1.0' );
    $resource->get->addAction( 'summary', 'libary_rr_summary' );

Now when a GET request is made to `http://my-site/api/1.0/books/summary.json`
or `http://my-site/api/1.0/books/<id>/summary.json` (where ID is a unique book
resource ID), a call will be made to `library_rr_summary` containing the GET
data and the ID, if relevant:
    
    library_rr_summary( $get_data, $id )
    
Now I can define the callback:
    
    function library_rr_summary( $data, $id ) {
      $summary = library_get_book_summary( $id );
      return array( 'book' => array( 'summary' => $summary ) );
    }

However there's a problem: this action has no meaning for the entire
collection of books, and the route `http://my-site/api/1.0/books/summary.json`
will hit this callback without an `$id`. This is a case where the action is
unsupported for a specific context and specifying that requires a small modification:

    function library_rr_summary( $data, $id ) {
      if (empty( $id ) { return RestfulResource::UNSUPPORTED; }
      $summary = library_get_book_summary( $id );
      return array( 'book' => array( 'summary' => $summary ) );
    }


