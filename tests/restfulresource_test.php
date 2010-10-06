<?php
require_once "simpletest/autorun.php";
require_once "../restfulresource_class.inc";
require_once "../restfulresource.module";

class RestfulResourceTest extends UnitTestCase {
  function testShouldCreateNewRestfulResource() {
    $rr = new RestfulResource('test', 'tests', 'testmodule');
    $this->assertIsA($rr, RestfulResource);
  }
  
  function testShouldHaveMethodSubClasses() {
    $rr = new RestfulResource('test', 'tests',  'testmodule');
    $this->assertIsA($rr->post, HTTPPost);
    $this->assertIsA($rr->get, HTTPGet);
    $this->assertIsA($rr->put, HTTPPut);
    $this->assertIsA($rr->delete, HTTPDelete);
  }
  
  function testShouldHaveMethodDefaultActions() {
    $rr = new RestfulResource('test', 'tests', 'testmodule');
    $this->assertNotNull($rr->post->actions['create']);
    $this->assertNotNull($rr->get->actions['read']);
    $this->assertNotNull($rr->put->actions['update']);
    $this->assertNotNull($rr->delete->actions['delete']);
  }
  
  function testShouldRemoveDefaultAction() {
    $rr = new RestfulResource('test', 'tests', 'testmodule');
    $rr->post->removeAction('create');
    $this->assertNull($rr->post->actions['create']);
  }

  function testShouldGenerateDrupalMenu() {
    $rr = new RestfulResource('test', 'tests', 'testmodule', '/api/1.1/');
    $rr2 = new RestfulResource('hand', 'hands', 'handmodule', '/api/');
    $menu = RestfulResource::generateMenu();
    
    $this->assertNotNull($menu['api/1.1/test.json']);
    $this->assertNotNull($menu['api/1.1/test.txt']);
    $this->assertNotNull($menu['api/1.1/tests/%']);
    $this->assertNotNull($menu['api/hand.json']);
    $this->assertNotNull($menu['api/hand.txt']);
    $this->assertNotNull($menu['api/hands/%']);
  }
}