<?php
class HTTPMethod {
  public function __construct($module) {
    $this->module = $module;
    $this->actions = array();
  }
  
  /**
   * Add a new action for this HTTP method
   *
   * @param  action
   *   name of the action
   * @param  callback <string>
   *   string name of the callback function (partial or complete)
   * @param full_callback
   *   whether to prepend the module name to the callback (defaults to false)
   */
  public function addAction( $action, $callback = null, $full_callback = false ) {
    if (empty( $callback )) {
      $callback = $action;
    }
    if ($full_callback == false && $this->module) {
      $this->actions[$action] = sprintf("%s_rr_%s", $this->module, $callback);
    }
    else {
      $this->actions[$action] = $callback;
    }
    return $this;
  }

  /**
   * Removes an action for this HTTP method
   *
   * @param  action
   *   name of the action to remove.
   */
  public function removeAction( $action ) {
    unset($this->actions[$action]);
    return $this;
  }
}
class HTTPPost extends HTTPMethod{}
class HTTPGet extends HTTPMethod{}
class HTTPPut extends HTTPMethod{}
class HTTPDelete extends HTTPMethod{}
class HTTPHead extends HTTPMethod{}

