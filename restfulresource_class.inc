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
    $this->post->removeAction($resource);
    $this->get->removeAction($resource);
    $this->put->removeAction($resource);
    $this->delete->removeAction($resource);
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
    $rr = self::$resources[$resource];

    switch ($_SERVER['REQUEST_METHOD']) {
      case "POST":
        break;
      case "GET":
        break;
      case "PUT":
        break;
      case "DELETE":
        break;
    }
  }
  
  /**
   * Figure out the action, id and response type from the path arguments
   *
   * @param  path_args
   *   what gets passed into route from the menu call.
   */
  public static function parsePath( $path_args, $resource, $method ) {
    $action = $resource;
    $id = null;
    $type = null;
    
    foreach($path_args as $arg) {
      $info = pathinfo($arg);
      // if there is an extension, stop. It's either an ID or action
      if ($info['extension']) {
        $type = $info['extension'];
        if (isset($method->actions[$info['filename']])) {
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
