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

?>

<div id="slides">
	<div class="slides_container">   
		<?php foreach ($elements as $key => $element) {?>
			<div>
				<a href="<?php print $element['link']; ?>" title="Перейти" target="_blank">
				<?php print $element['render_img']?>
				</a>
				<div class="caption">
					<?php print $element['description']; ?>
				</div>
			</div>
			<?php }?>          
	</div>
		  <a href="#" class="prev"></a>
	  <a href="#" class="next"></a>
</div>