<?php 
/**
 * Создаем блок слайдера
 *
 * @params $elements
 *	Содержит массив всех элементов блока.
 *  Доступные ключи
 *   img_fid - Ид файла слайдера
 *	 description - описание изображения
 *   link - ссылка изображения
 *   url_img - путь изображения
 *   render_img - полная картинка с изображением (с применением стиля)
 *
 * @see sliders_block_view()
 */
//drupal_set_message('<pre>' . print_r($elements, 1) . '</pre>');
$count_slider = count($elements);
?>

     <div id="slider">
        <div class="main_view">
            <div class="window">
                <div class="image_reel">
 				<?php
            	 foreach ($elements as $key => $element) {
            	 	print '<a href="' . $element['link'] . '" title="" >' . $element['render_img'] . '</a>';  
            	 }
              ?>

                </div>
                <div class="descriptions">

<?php
               foreach ($elements as $key => $element) {
                    print '<div class="desc" style="display: none;">';
                    print $element['description'];
                    print '</div>';
                }
?>
                </div>
            </div>
        
            <div class="paging">
            <?php 
  				for($i=0;$i<$count_slider;$i++){
                    $s=$i+1;
                    print '<a rel="'.$s.'" href="#">'.$s.'</a>';
                }
            ?>

            </div>
        </div>
      </div><!-- EOF: #banner -->