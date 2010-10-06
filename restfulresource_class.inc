<?php
class RestfulResource {
  public function __construct( $resource, $module ) {
    $this->get = new HTTPGet;
    $this->post = new HTTPPost;
    $this->put = new HTTPPut;
    $this->delete = new HTTPDelete;
  
    $this->post->addAction('create', sprintf("%s_rr_create", $module));
    $this->get->addAction('read', sprintf("%s_rr_read", $module));
    $this->put->addAction('update', sprintf("%s_rr_update", $module));
    $this->delete->addAction('delete', sprintf("%s_rr_delete", $module));
  }
}

class HTTPMethod {
  public $actions;
  
  public function addAction( $action, $callback ) {
    $this->actions[$action] = $callback;
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
