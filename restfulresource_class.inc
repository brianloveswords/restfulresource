<?php
require_once 'http_method_class.inc';
require_once 'http_response_class.inc';

class RestfulResource {
  const UNAUTHORIZED = 'unauthorized';
  const UNSUPPORTED = 'unsupported';
  const NOT_FOUND = 'not_found';
  
  public static $resources;
  
  public function __construct( $resource, $prefix = '', $module = null ) {
    $this->module = $module;
    $this->resource = $resource;
    $this->prefix = $prefix;
    
    $this->get = new HTTPGet($module);
    $this->post = new HTTPPost($module);
    $this->put = new HTTPPut($module);
    $this->delete = new HTTPDelete($module);
    $this->head = new HTTPHead($module);
    
    $this->post->addAction($resource, "create");
    $this->get->addAction($resource, "read");
    $this->put->addAction($resource, "update");
    $this->delete->addAction($resource, "delete");
    $this->head->addAction($resource, "exists");
    
    self::$resources[$resource] = $this;
  }

  public function removeDefaultActions() {
    $this->post->removeAction($this->resource);
    $this->get->removeAction($this->resource);
    $this->put->removeAction($this->resource);
    $this->delete->removeAction($this->resource);
    $this->head->removeAction($this->resource);
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
      $offset = 0;
      $prefix = trim( $r->prefix, '/' );
      
      if (strlen($prefix) > 0) {
        $offset = count(explode('/', $prefix));
        $prefix .= '/';
      }
      
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
   * @param  phpinput
   *   the contents of 'php://input'. it must be collected from the first
   *   method after the request, so the pre-router passes it in 
   */
  public static function route( $resource, $path_args, $phpinput = '' ) {
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

    $_PUT = self::decode( $type, $phpinput );
    
    $data = array(
      'get' => $_GET,
      'post' => $_POST ? $_POST : $_PUT, //activeresource puts posts in puts.
      'put' => $_PUT,
      'delete' => array(),
      'head' => array(),
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
    else if ($result === self::UNSUPPORTED) {
      HTTPResponse::badRequest();
      // exits
    }
    else if (is_array($result) && empty( $result['errors'] ) == false) {
      HTTPResponse::unprocessable( self::encode ($type, $result) );
      // exits
    }
    else if ( $http_method == 'post' ) {
      HTTPResponse::created( $result );
      // exits
    }
    else if ( $http_method == 'put' || $http_method == 'head' ) {
      HTTPResponse::ok( );
      // exits
    }
    else {
      HTTPResponse::ok( self::encode($type, $result) );
      // exits
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
   *   decoded object
   */
  private static function decode( $type, $data ) {
    $result = $data;
    switch ($type) {
      case 'json':
        $result = json_decode($data);
        break;
      case 'txt':
        parse_str($data, $result);
        break;
    }
    return (object)$result;
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
