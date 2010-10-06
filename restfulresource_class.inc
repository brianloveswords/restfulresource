<?php
class RestfulResource {
  public static $resources;
  
  public function __construct( $resource, $resource_plural, $module, $prefix = '' ) {
    $this->module = $module;
    $this->resource = $resource;
    $this->resource_plural = $resource_plural;
    $this->prefix = $prefix;
    
    $this->get = new HTTPGet;
    $this->post = new HTTPPost;
    $this->put = new HTTPPut;
    $this->delete = new HTTPDelete;
  
    $this->post->addAction('create', sprintf("%s_rr_create", $module));
    $this->get->addAction('read', sprintf("%s_rr_read", $module));
    $this->put->addAction('update', sprintf("%s_rr_update", $module));
    $this->delete->addAction('delete', sprintf("%s_rr_delete", $module));
    
    self::$resources[$resource] = $this;
  }

  public function removeDefaultActions() {
    $this->post->removeAction('create');
    $this->get->removeAction('read');
    $this->put->removeAction('update');
    $this->delete->removeAction('delete');
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
      $arguments_offset = substr_count($prefix, '/');
      $prefix .= strlen($prefix) > 0 ? '/' : '';
    
      $template = array(
        'description' => 'API router',
        'page callback' => 'restfulresource_router',
        'page arguments' => array($r->resource),
        'access callback' => TRUE,
        'type' => MENU_CALLBACK,
      );
        
      $routes[sprintf("%s%s.json", $prefix, $r->resource)] = $template;
      $routes[sprintf("%s%s.txt", $prefix, $r->resource)] = $template;
      $routes[sprintf("%s%s/%%", $prefix, $r->resource_plural)] = $template;
    }    
    return $routes;
  }

  /**
   * Routes a menu call to the proper method.
   *
   * @param  resource
   *   the name of the resource
   */
  public static function route( $resource ) {
    $path_args = func_get_args();
    self::$resources[$resource];
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

class HTTPPost extends HTTPMethod{
}
class HTTPGet extends HTTPMethod{
}
class HTTPPut extends HTTPMethod{
}
class HTTPDelete extends HTTPMethod{
}
