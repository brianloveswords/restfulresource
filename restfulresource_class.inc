<?php
class RestfulResource {
  const UNAUTHORIZED = 'unauthorized';
  const NOT_FOUND = 'not_found';
  
  public static $resources;
  
  public function __construct( $resource, $module, $prefix = '' ) {
    $this->module = $module;
    $this->resource = $resource;
    $this->prefix = $prefix;
    
    $this->get = new HTTPGet($this);
    $this->post = new HTTPPost($this);
    $this->put = new HTTPPut($this);
    $this->delete = new HTTPDelete($this);
    
    $this->post->addAction($resource, sprintf("%s_rr_create", $module));
    $this->get->addAction($resource, sprintf("%s_rr_read", $module));
    $this->put->addAction($resource, sprintf("%s_rr_update", $module));
    $this->delete->addAction($resource, sprintf("%s_rr_delete", $module));
    
    self::$resources[$resource] = $this;
  }

  public function removeDefaultActions() {
    $this->post->removeAction($this->resource);
    $this->get->removeAction($this->resource);
    $this->put->removeAction($this->resource);
    $this->delete->removeAction($this->resource);
    return $this;
  }
  
  /**
   * Generates a drupal menu (routes) for this resource.
   *
   * @param   prefix
   *   if given, a prefix to use for the routes.
   * @return 
   *   array of routes that can be returned from hook_menu
   */
  public static function generateMenu( ) {
    $routes = array();
    foreach (self::$resources as $r) {
      $prefix = trim( $r->prefix, '/' );
      $offset = substr_count($prefix, '/');
      $prefix .= strlen($prefix) > 0 ? '/' : '';
    
      $template = array(
        'description' => 'API router',
        'page callback' => 'restfulresource_router',
        'page arguments' => array($r->resource, $offset),
        'access callback' => TRUE,
        'type' => MENU_CALLBACK,
      );
      $all_by_json = sprintf("%s%s.json", $prefix, $r->resource);
      $all_by_text = sprintf("%s%s.txt", $prefix, $r->resource);
      $actions = sprintf("%s%s/%%", $prefix, $r->resource);
      
      $routes[$all_by_json] = $template;
      $routes[$all_by_text] = $template;
      $routes[$actions] = $template;
      $routes[$actions]['page arguments'] = array($r->resource, $offset + 1, $offset + 2);
    }    
    return $routes;
  }

  /**
   * Routes a menu call to the proper method.
   *
   * @param  resource
   *   the name of the resource
   * @param  path_args
   *   the extra arguments passed when calling the resource
   */
  public static function route( $resource, $path_args ) {
    $http_method = strtolower($_SERVER['REQUEST_METHOD']);
    $rr = self::$resources[$resource];
    
    $pathparts = self::parsePath( $path_args, $rr, $http_method );
    $action = $pathparts['action'];
    $id = $pathparts['id'];
    $type = $pathparts['type'];
    
    $callback = $rr->$http_method->actions[$action];
    
    self::header( $type );
    
    // if the callback doesn't exit, execution stops immediately.
    if (function_exists( $callback ) == false) {
      HTTPResponse::badRequest();
      // exits
    }

    $_PUT = self::decode( $type, file_get_contents('php://input') );
    
    $data = array(
      'get' => $_GET,
      'post' => $_POST ? $_POST : $_PUT, //activeresource puts posts in puts.
      'put' => $_PUT,
      'delete' => array(),
    );

    $result = call_user_func( $callback, $data[$http_method], $id );
    if ($result === self::NOT_FOUND) {
      HTTPResponse::notFound();
      // exits
    }
    else if ($result === self::UNAUTHORIZED) {
      HTTPResponse::unauthorized();
      // exits
    }
    else if (isset( $result['errors'] )) {
      HTTPResponse::unprocessable( self::encode ($type, $result) );
      // exits
    }
    else if ( $type == 'post' ) {
      HTTPResponse::created( $result );
    }
    else {
      HTTPResponse::ok( self::encode($type, $result) );
    }
  }
  
  /**
   * Figure out the action, id and response type from the path arguments
   *
   * @param  path_args
   *   what gets passed into route from the menu call.
   * @param  resource
   *   the resource object
   * @param  http_method
   *   the method object from the resource for the type of request
   */
  public static function parsePath( $path_args, $resource, $http_method) {
    $action = $resource->resource;
    $id = null;
    $type = null;
    $method = $resource->$http_method;
    
    foreach($path_args as $arg) {
      $info = pathinfo($arg);
      // if there is an extension, stop. It's either an ID or action
      if ($info['extension']) {
        $type = $info['extension'];
        if (isset( $method->actions[$info['filename']] )) {
          $action = $info['filename'];
        }
        else {
          $id = $info['filename'];
        }
        break;
      }
      else {
        $id = $arg;
      }
    }
    return array('action' => $action, 'id' => $id, 'type' => $type);
  }

  /**
   * Methods for decoding different types of incoming requests.
   *
   * @param  type
   *   the incoming type (txt or json currently)
   * @param  data
   *   the incoming data
   * @return
   *   decoded object as an array
   */
  private static function decode( $type, $data ) {
    $result = $data;
    switch ($type) {
      case 'json':
        $result = json_decode(file_get_contents('php://input'));
        break;
      case 'txt':
        parse_str(file_get_contents('php://input'), $result);
        break;
    }
    return (array)$result;
  }

  /**
   * Methods for encoding responses
   *
   * @param  type
   *   the incoming type (txt or json currently)
   * @param  data
   *   the incoming data
   * @return
   *   encoded object as a string
   */
  private static function encode( $type, $data ) {
    $result = $data;
    switch ($type) {
      case 'json':
        $result = json_encode($data);
        break;
      case 'txt':
        $result = print_r($data, true);
        break;
    }
    return (string)$result;
  }

  /**
   * Set response header for type
   *
   * @param  type
   *   the reponse type 
   */
  private static function header( $type ) {
    switch ($type) {
      case 'json':
        drupal_set_header('Content-Type: text/javascript; charset=utf-8');
        break;
      case 'txt':
        drupal_set_header('Content-Type: text/plain; charset=utf-8');
        break;
    }
  }
}

class HTTPMethod {
  public $actions;
  
  public function addAction( $action, $callback ) {
    $this->actions[$action] = $callback;
    return $this;
  }

  public function removeAction( $action ) {
    unset($this->actions[$action]);
    return $this;
  }
}
class HTTPPost extends HTTPMethod{}
class HTTPGet extends HTTPMethod{}
class HTTPPut extends HTTPMethod{}
class HTTPDelete extends HTTPMethod{}

class HTTPResponse {
  public function badRequest() {
    drupal_set_header('HTTP/1.1 400 Bad Request');
    exit;
  }
  public function notFound() {
    drupal_set_header('HTTP/1.1 404 Not Found');
    exit;
  }
  public function ok( $body ) {
    drupal_set_header('HTTP/1.1 200 OK');
    print $body;
    exit;
  }
  public function created( $location ) {
    drupal_set_header('HTTP/1.1 201 Created');
    drupal_set_header(sprintf('Location: %s/%d', $location));
    exit;
  }
  public function unprocessable( $body ) {
    drupal_set_header('HTTP/1.1 422 Unprocessable Entity');
    print $body;
    exit;
  }
  public function unauthorized() {
    drupal_set_header('HTTP/1.1 401 Unauthorized');
    exit;
  }
}