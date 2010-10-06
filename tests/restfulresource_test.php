<?php
require_once "simpletest/autorun.php";
require_once "../restfulresource_class.inc";

class RestfulResourceTest extends UnitTestCase {
  function testShouldCreateNewRestfulResource() {
    $rr = new RestfulResource('test', 'tests', 'test');
    $this->assertIsA($rr, RestfulResource);
  }
  
  function testShouldHaveMethodSubClasses() {
    $rr = new RestfulResource('test', 'tests',  'test');
    $this->assertIsA($rr->post, HTTPPost);
    $this->assertIsA($rr->get, HTTPGet);
    $this->assertIsA($rr->put, HTTPPut);
    $this->assertIsA($rr->delete, HTTPDelete);
  }
  
  function testShouldHaveMethodDefaultActions() {
    $rr = new RestfulResource('test', 'tests', 'test');
    $this->assertNotNull($rr->post->actions['create']);
    $this->assertNotNull($rr->get->actions['read']);
    $this->assertNotNull($rr->put->actions['update']);
    $this->assertNotNull($rr->delete->actions['delete']);
  }
  
  function testShouldRemoveDefaultAction() {
    $rr = new RestfulResource('test', 'tests', 'test');
    $rr->post->removeAction('create');
    $this->assertNull($rr->post->actions['create']);
  }

  function testShouldGenerateDrupalMenu() {
    $rr = new RestfulResource('test', 'tests', 'test');
    $menu = $rr->generateMenu('/api/1.1/');
    
    $this->assertNotNull($menu['api/1.1/test.json']);
    $this->assertNotNull($menu['api/1.1/test.txt']);
    $this->assertNotNull($menu['api/1.1/tests/%']);
    $this->assertNotNull($menu['api/1.1/tests/%/%']);
  }
}