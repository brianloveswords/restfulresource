<?php
require_once "simpletest/autorun.php";
require_once "../restfulresource_class.inc";
require_once "../restfulresource.module";

class RestfulResourceTest extends UnitTestCase {
  function testShouldCreateNewRestfulResource() {
    $rr = new RestfulResource('tests', 'testmodule');
    $this->assertIsA($rr, RestfulResource);
  }
  
  function testShouldHaveMethodSubClasses() {
    $rr = new RestfulResource('tests',  'testmodule');
    $this->assertIsA($rr->post, HTTPPost);
    $this->assertIsA($rr->get, HTTPGet);
    $this->assertIsA($rr->put, HTTPPut);
    $this->assertIsA($rr->delete, HTTPDelete);
  }
  
  function testShouldHaveMethodDefaultActions() {
    $rr = new RestfulResource('tests', 'testmodule');
    $this->assertNotNull($rr->post->actions['tests']);
    $this->assertNotNull($rr->get->actions['tests']);
    $this->assertNotNull($rr->put->actions['tests']);
    $this->assertNotNull($rr->delete->actions['tests']);
  }
  
  function testShouldRemoveDefaultAction() {
    $rr = new RestfulResource('tests', 'testmodule');
    $rr->removeDefaultActions();
    $this->assertNull($rr->post->actions['tests']);
  }

  function testShouldGenerateDrupalMenu() {
    $rr = new RestfulResource('tests', 'testmodule', '/api/1.1/');
    $rr2 = new RestfulResource('hands', 'handmodule', '/api/');
    $menu = RestfulResource::generateMenu();
    
    $this->assertNotNull($menu['api/1.1/tests.json']);
    $this->assertNotNull($menu['api/1.1/tests.txt']);
    $this->assertNotNull($menu['api/1.1/tests/%']);
    $this->assertNotNull($menu['api/hands.json']);
    $this->assertNotNull($menu['api/hands.txt']);
    $this->assertNotNull($menu['api/hands/%']);
  }

  function testShouldRouteToGetRead() {
    $_SERVER['REQUEST_METHOD'] = 'GET';
    function bagelmodule_rr_read() { return 'ok'; } 
    $rr = new RestfulResource('bagels', 'bagelmodule');
    
    $response = RestfulResource::route('bagels', array('bagels.txt'));
    $this->assertEqual($response, 'ok');
  }

  function testShouldParsePathArguments() {
    $rr = new RestfulResource('bagels', 'bagelmodule');
    $rr->get->addAction('summary', 'bagelmodule_summary');
    
    $default_read_all = array('bagels.txt');
    $default_read_one = array('1.json');
    $action_on_all = array('summary.txt');
    $action_on_one = array('10100', 'summary.txt');
    $get_named_id = array('bogus.txt');

    $response = RestfulResource::parsePath($default_read_all, $rr, 'get');
    $this->assertEqual($response['action'], 'bagels');
    $this->assertEqual($response['id'], null);
    $this->assertEqual($response['type'], 'txt');
    
    $response = RestfulResource::parsePath($default_read_one, $rr,  'get');
    $this->assertEqual($response['action'], 'bagels');
    $this->assertEqual($response['id'], '1');
    $this->assertEqual($response['type'], 'json');
    
    $response = RestfulResource::parsePath($action_on_all, $rr,  'get');
    $this->assertEqual($response['action'], 'summary');
    $this->assertEqual($response['id'], null);
    $this->assertEqual($response['type'], 'txt');
    
    $response = RestfulResource::parsePath($action_on_one, $rr,  'get');
    $this->assertEqual($response['action'], 'summary');
    $this->assertEqual($response['id'], '10100');
    $this->assertEqual($response['type'], 'txt');
    
    $response = RestfulResource::parsePath($get_named_id, $rr,  'get');
    $this->assertEqual($response['action'], 'bagels');
    $this->assertEqual($response['id'], 'bogus');
    $this->assertEqual($response['type'], 'txt');
  }
}