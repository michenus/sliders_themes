<?php 
/**
 * @file 
 * UI настройки модуля слайдера
 */

/**
 * Конструктор форм.
 *
 * Здесь происходит загрузка элементов слайдеров, их описание и отображения
 *  показываются также и превью изображений
 * @ingroup forms
 * @see system_settings_form
 */
function _slider_settings_form() {
  //показывать ли превью при просмотре
  $sliders_previews = variable_get('sliders_previews', 1);
  //количество записей в таблице со слайдером
  $query = db_select('sliders_themes');
  $query->addExpression('COUNT(*)');
  $sliders_img_count = $query->execute()->fetchField();
  // Wrap global setting fieldsets in vertical tabs.
  $form['general'] = array(
    '#type' => 'vertical_tabs',
    '#prefix' => '<h2><small>' . t('Настройки блока слайдера!') . '</small></h2>',
    '#description' => t('Для каждого изображения -> своя вкладка'),
  );

  // Глобальные настройки (выбор стиля изображения, ).
  $form['global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Общие настройки'),
    '#group' => 'general',
  );

  $form['global']['preview'] = array(
    '#type' => 'checkbox',
    '#title' => t('Показывать превью изображений?'),
    '#default_value' => $sliders_previews,
    '#description' => t('Выбирете этот пункт, если хотите видеть превью изображений. Изображения показываются c примененим выбранного стиля.'),
  );

  $styles = image_styles();
  $options_style = array();
  foreach ($styles as $key => $style) {
    $options_style[$key] = $style['label'];
  }

  $form['global']['styles'] = array(
    '#type' => 'select',
    '#title' => t('Стиль изображений'),
    '#options' => $options_style,
    '#default_value' => variable_get('sliders_img_style', 'large'),
    '#description' => t('Выбирете стиль изображений для превью и отображения в блоке слайдера. По умолчанию используется "Large (480x480)"'),
  );

  $form['global']['add_img'] = array(
    '#type' => 'submit',
    '#name' => 'add_img',
    '#value' => t('Добавить изображение'),
    '#suffix' => '<span>' . t('Количество изображений: @count',  array('@count' => $sliders_img_count)) . '</span>',
    '#submit' => array('_sliders_add_img'), //Если отключен ajax
  );
  //Получение списка изображений и данных по ним
  $slider_images = db_select('sliders_themes', 'si')
                    ->fields('si')
                    ->orderBy('si.slider_weight', 'ASC')
                    ->execute()
                    ->fetchAll();
  $index = 1;
  foreach ($slider_images as $key => $s_img) {
   // Глобальные настройки (выбор стиля изображения, ).
      $form['sliders_img_' . $index] = array(
        '#type' => 'fieldset',
        '#title' => t('@img изображение', array('@img' => $index)),
        '#group' => 'general',
      );
      // Показываем превью, если изображение загружено или поставлен пункт "показывать превью"
      // проверим, загружено ли изображение
      $img_preview = $s_img->slider_image_id;//;variable_get('slidres_img_' . $i, NULL);
      $form['sliders_img_' . $index]['sliders_weight_' . $index] = array(
          '#type' => 'weight',
          '#title' => t('Вес изображения.'),
          '#default_value' => $s_img->slider_weight,
          '#delta' => 10,
          '#description' => t('Очередь изображения, чем больше - тем дальше в очереди.'),
      );
      if ($sliders_previews && $img_preview != NULL) {
         // опции загрузки изображения
        $file_img = file_load($img_preview);
         $form['sliders_img_' . $index]['preview_' . $index] = array(
          '#type' => 'html_tag',
          '#tag' => 'div',
          '#value' => theme('image_style', array(
                        'style_name' => variable_get('sliders_img_style', 'large'),
                        'path'       => $file_img->uri,
                        'alt'        => t('Превью изображения № @img', array('@img' => $index) ),
                        'title'      => t('Превью изображения № @img', array('@img' => $index) ), 
                    )),
          );
      }// Превью


      // Загрузчик изображения 
      $form['sliders_img_' . $index]['slider_file_' . $index] = array(
        '#type' => 'managed_file',
        '#title' => t('Загрузить/изменить изображение № @img', array('@img' => $index) ),
        '#description' => t('Измените или загрузите текущие изображание'),
        '#default_value' => ($img_preview != NULL) ? $img_preview : 0,
        '#upload_location' => 'public://sliders/',
        '#upload_validators' => array(
            'file_validate_extensions' => array('gif png jpg jpeg'),
            // Pass the maximum file size in bytes
            'file_validate_size' => array(2 * 1024 * 1024),
        ),
      );

      //Остальной описание
        $form['sliders_img_' . $index]['sliders_descriptoin_' . $index] = array(
          '#title' => t('Описание изображения'),
          '#type' => 'textarea',
          '#description' => t('Добавьте описание изображения, оно будет использоваться в описание элемента слайдера.'),
          '#default_value' => $s_img->slider_description,
        );
        $form['sliders_img_' . $index]['sliders_link_' . $index] = array(
          '#title' => t('Ссылка изображения'),
          '#type' => 'textfield',
          '#description' => t('Добавьте ссылку для элемента слайдера. Она используется в тексте "подробнее". Если начинается с // - то выводиться как глобальная ссылка. Если без первого слеша - как локальная. Например: "//vk.com" выведит как есть, а vk.com выведет как /vk.com.'),
          '#default_value' => $s_img->slider_link,
        );
       // Сохранить текущий номер изображения с помощью скрытого поля формы
       $form['sliders_img_' . $index]['number_' . $index] = array('#type' => 'hidden', '#value' => $index);
       $form['sliders_img_' . $index]['slider_id_' . $index] = array('#type' => 'hidden', '#value' => $s_img->id);
       // Кнопка удаление изображения
       $form['sliders_img_' . $index]['delete'] = array(
        '#type' => 'submit',
        '#name' => 'delete_img_' . $index,
        '#value' => t('Удалить изображение'),
        '#submit' => array('_sliders_delete_img'), //
       );

      $index++;
  }

  $form['submit']  = array(
    '#type' => 'submit',
    '#name' => 'sliders_save',
    '#value' => t('Сохранить все изменения'),
    '#submit' => array('_slider_settings_form_submit'),
  );

  $form['#attributes'] = array('id' => 'slider_settings_form');
  $form['link'] = array(
    '#markup' => '<a href="/admin/structure/sliders/preview">' . t('Превью') . '<a>', 
    );
  return $form;
}

/**
 * Функция добавления изображения в слайдер
 */
function _sliders_add_img($form, &$form_state) {
  // Добавляем пустой слайдер
  db_insert('`sliders_themes`')
  ->fields(array('slider_image_id', 'slider_description', 'slider_link', 'slider_weight'))
  ->values(array(
      'slider_image_id' => NULL,
      'slider_description' => '',
      'slider_link' => '',
      'slider_weight' => 0
      ))
  ->execute();
  // Обновляем форму
  $form_state['rebuild'] = TRUE;
  drupal_set_message( t('Изображение добавлено') );

}

/**
 * Реализуем удаление изображения
 */
function _sliders_delete_img($form, &$form_state) {
  $values = $form_state['values'];
  //dpm( $values['slider_id'] ) ;
  //$slider_id = (int)$values['slider_id'];
  $number_img = str_replace('edit-sliders-img-', '', $values['general__active_tab']);
  $slider_id = (int)$values['slider_id_' . $number_img];
  try {
      db_delete('`sliders_themes`')
      ->condition( 'id',  $slider_id)
      ->execute();
  } catch (Exception $e) {
    drupal_set_message( t('Ошибка работы с базой данных.'), 'error' );
  }
  drupal_set_message( t('Выбранное изображение удалено') );
}

/**
 * Implementation of hook_settings_form_submit().
 */
function _slider_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  $settings = count($form_state['groups']['general']) - 2;

  variable_set('sliders_previews', $values['preview']);
  variable_set('sliders_img_style', $values['styles']);

  for ($i=1; $i <= $settings; $i++) { 
     // значение id изображения в базе
     $slider_id = (int) $values['slider_id_' . $i];
      // Сохранение изображения 
     // новое значение веса
     $sliders_weight = (int) $values['sliders_weight_' . $i];
     // новое значен(ие ид файла
     $slider_file = ( $values['slider_file_' . $i] == '0') ? NULL : (int) $values['slider_file_' . $i];

     // новое значение описания
     $sliders_descriptoin  = $values['sliders_descriptoin_' . $i];
     // новое значение ссылки
     $sliders_link  = $values['sliders_link_' . $i];
    
      $file = file_load( $slider_file );
        // Если статус действительно "Временно", то...
        if ($file->status == 0) {
          // Устанавливаем нормальный статус.
          $file->status = FILE_STATUS_PERMANENT;
          // Сохраняем наш файл.
          file_save($file);
          file_usage_add($file, 'sliders', 'sliders-' . $i, $slider_file);
        }

        db_update('`sliders_themes`')
        ->fields(array(
          'slider_image_id' => $slider_file,
          'slider_description' => $sliders_descriptoin,
          'slider_link' => $sliders_link,
          'slider_weight' => $sliders_weight,
          ))
        ->condition('id', $slider_id)
        ->execute();
  }
  
   drupal_set_message( t('Изменения сохранились') );
}