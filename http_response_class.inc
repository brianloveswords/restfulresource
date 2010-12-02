<?php
class HTTPResponse {
  /**
   * Sets a header for 401 Bad Request and exits
   */
  public function badRequest() {
    drupal_set_header('HTTP/1.1 400 Bad Request');
    exit;
  }
  /**
   * Sets a header for 404 Not Found and exits
   */
  public function notFound() {
    drupal_set_header('HTTP/1.1 404 Not Found');
    exit;
  }
  /**
   * Sets a header for 200 OK, prints the body and exits
   *
   * @param  body
   *   content to print in the response
   */
  public function ok( $body = null ) {
    drupal_set_header('HTTP/1.1 200 OK');
    if (!empty($body)) { print $body; }
    exit;
  }
  /**
   * Sets a header for 201 Created, a Location header describing where to
   * find the new resource, and exits.
   *
   * @param  location
   *   full (or relative) url where to find the new resource
   */
  public function created( $location ) {
    drupal_set_header('HTTP/1.1 201 Created');
    drupal_set_header(sprintf('Location: %s', $location));
    exit;
  }
  /**
   * Sets a header for 422 Unprocessable Entity (errors in the input) and
   * exits.
   *
   * @param  body
   *   content to print in the response. 
   */
  public function unprocessable( $body ) {
    drupal_set_header('HTTP/1.1 422 Unprocessable Entity');
    print $body;
    exit;
  }
  /**
   * Sets a header for 401 Unauthorized and exits.
   */
  public function unauthorized() {
    drupal_set_header('HTTP/1.1 401 Unauthorized');
    exit;
  }
}