<?php
require_once "simpletest/autorun.php";
require_once "../restfulresource_class.inc";

class RestfulResourceTest extends UnitTestCase {
  function testShouldCreateNewRestfulResource() {
    $rr = new RestfulResource('test', 'test');
    $this->assertIsA($rr, RestfulResource);
  }
  
  function testShouldHaveMethodSubClasses() {
    $rr = new RestfulResource('test', 'test');
    $this->assertIsA($rr->post, HTTPPost);
    $this->assertIsA($rr->get, HTTPGet);
    $this->assertIsA($rr->put, HTTPPut);
    $this->assertIsA($rr->delete, HTTPDelete);
  }
  
  function testShouldHaveMethodDefaultActions() {
    $rr = new RestfulResource('test', 'test');
    $this->assertNotNull($rr->post->actions['create']);
    $this->assertNotNull($rr->get->actions['read']);
    $this->assertNotNull($rr->put->actions['update']);
    $this->assertNotNull($rr->delete->actions['delete']);
  }
}