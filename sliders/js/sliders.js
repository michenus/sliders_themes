//<?php 
/**
 * @file 
 * Запуск слайдера с использование текущего плагина
 */
// ?>
(function($){
  $(document).ready(function() {
    $('#slides').slides({
        preload: true,
        play: 5000,
        pause: 2500,
        hoverPause: true
      });
  });
})(jQuery);