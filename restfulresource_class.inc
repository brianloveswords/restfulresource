<?php
class RestfulResource {
  public static $resources;
  
  public function __construct( $resource, $resource_plural, $module ) {
    $this->module = $module;
    $this->resource = $resource;
    $this->resource_plural = $resource_plural;
    
    $this->get = new HTTPGet;
    $this->post = new HTTPPost;
    $this->put = new HTTPPut;
    $this->delete = new HTTPDelete;
  
    $this->post->addAction('create', sprintf("%s_rr_create", $module));
    $this->get->addAction('read', sprintf("%s_rr_read", $module));
    $this->put->addAction('update', sprintf("%s_rr_update", $module));
    $this->delete->addAction('delete', sprintf("%s_rr_delete", $module));
  }

  /**
   * Generates a drupal menu (routes) for this resource.
   *
   * @param   prefix
   *   if given, a prefix to use for the routes.
   * @return 
   *   array of routes that can be returned from hook_menu
   */
  public function generateMenu( $prefix = '' ) {
    $prefix = trim( $prefix, '/' );
    $arguments_offset = substr_count($prefix, '/');
    $prefix .= strlen($prefix) > 0 ? '/' : '';
    
    $template = array(
      'description' => 'API router',
      'page callback' => 'restfulresource_router',
      'page arguments' => array($this->resource),
      'access callback' => TRUE,
      'type' => MENU_CALLBACK,
    );
    $routes = array(
      sprintf("%s%s.json", $prefix, $this->resource) => $template,
      sprintf("%s%s.txt", $prefix, $this->resource) => $template,
      sprintf("%s%s/%%", $prefix, $this->resource_plural) => $template,
      sprintf("%s%s/%%/%%", $prefix, $this->resource_plural) => $template,
    );
    return $routes;
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
