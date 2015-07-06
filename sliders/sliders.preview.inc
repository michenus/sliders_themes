<?php
/**
 * @file
 * Файл предосмтора слайдера.
 *
 * Предоставлеяет предосмотр слайдера после изменений css или js
 * помогает смотреть блок слайдера и проверять его работоспособность.
 */
/*
* Страница предосмотра
*/
function _slider_settings_preview() {
   $block = block_load('sliders', 'sliders');      
   $output =_block_get_renderable_array(_block_render_blocks(array($block))); 
   return  drupal_render($output);
}