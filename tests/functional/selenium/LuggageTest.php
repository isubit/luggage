<?php
/**
 * @author Ryan Frahm
 * @date 10/6/14
 * @time 12:35 PM
 */

class LuggageTest extends PHPUnit_Extensions_Selenium2TestCase {
  protected function setUp() {
    $this->setBrowser('firefox');
    $this->setBrowserUrl('http://local.dev/luggage');
  }

  /**
   * Tests the title
   */
  public function testTitle() {
    $title = 'Welcome to luggage | luggage';
    $this->url('/');
    $this->assertEquals( $title, $this->title());
  }

  /**
   * Tests that the categories button exists
   */
  public function testLogin() {
    $path = 'luggage/categories';
    $this->url('/');
    $element = $this->byXPath('//*[@id="main-menu-links"]/li/a')->click();
    $this->assertStringEndsWith('luggage/categories', $this->url());
  }
}