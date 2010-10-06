<?php
class RestfulResource {
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
    $type = $pathparts['method'];
    $callback = $rr->$http_method->actions[$action];
    $result = call_user_func( $callback, $id );
    
    return $result;
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
