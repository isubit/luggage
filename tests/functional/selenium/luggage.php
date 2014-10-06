<?php
/**
 * Created by PhpStorm.
 * User: rlfrahm
 * Date: 10/6/14
 * Time: 12:35 PM
 */

class LuggageTestCase extends PHPUnit_Extensions_Selenium2TestCase {
  protected function setUp() {
    $this->setBrowser('firefox');
    $this->setBrowserUrl('http://local.dev/luggage');
  }

  /**
   * Tests the title
   */
  public function testtitle() {
    $title = 'luggage';
    $this->url('http://local.dev/luggage');
    $this->assertEquals( $title, $this->title());
  }
}