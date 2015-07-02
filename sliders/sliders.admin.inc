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

 $sliders_previews = variable_get('sliders_previews', 1);

 $sliders_img_count = variable_get('sliders_img_count', 0);

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

//drupal_set_message('<pre>' . print_r($styles, 1) . '</pre>');
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

  //Отображение всех изображений, если их больше одного 
  if ($sliders_img_count > 0) {
    for ($i = 1; $i <= $sliders_img_count ; $i++) { 
      // Глобальные настройки (выбор стиля изображения, ).
      $form['sliders_img_' . $i] = array(
        '#type' => 'fieldset',
        '#title' => t('@img изображение', array('@img' => $i)),
        '#group' => 'general',
      );

      // Показываем превью, если изображение загружено или поставлен пункт "показывать превью"
      // проверим, загружено ли изображение
      $img_preview = variable_get('slidres_img_' . $i, NULL);
      
      if ($sliders_previews && $img_preview != NULL) {
         // опции загрузки изображения
        $file_img = file_load($img_preview);
         $form['sliders_img_' . $i]['preview_' . $i] = array(
          '#type' => 'html_tag',
          '#tag' => 'div',
          '#value' => theme('image_style', array(
                        'style_name' => variable_get('sliders_img_style', 'large'),
                        'path'       => $file_img->uri,
                        'alt'        => t('Превью'),
                        'title'      => t('Превью'),
                    )),
          );
      }// Превью
      // Загрузчик изображения 
      $form['sliders_img_' . $i]['slider_file_' . $i] = array(
        '#type' => 'managed_file',
        '#title' => t('Загрузить/изменить изображение № @img', array('@img' => $i) ),
        '#description' => t('Измените или загрузите текущие изображание'),
        '#default_value' => ($img_preview != NULL) ? $img_preview : 0,
        '#upload_location' => 'public://sliders/',
        '#upload_validators' => array(
            'file_validate_extensions' => array('gif png jpg jpeg'),
            // Pass the maximum file size in bytes
            'file_validate_size' => array(2048),
        ),
      );
      //Остальной описание
      $form['sliders_img_' . $i]['sliders_descriptoin_' . $i] = array(
        '#title' => t('Описание изображения'),
        '#type' => 'textarea',
        '#description' => t('Добавьте описание изображения, оно будет использоваться ввиде описание к элементу слайдера и к описанию изображения'),
        '#default_value' => variable_get('sliders_descriptoin_' . $i, ''),
      );
      $form['sliders_img_' . $i]['sliders_link_' . $i] = array(
        '#title' => t('Ссылка изображения'),
        '#type' => 'textfield',
        '#description' => t('Добавьте ссылку для элемента слайдера. Она используется в тексте "подробнее". Если начинается с // - то выводиться как глобальная ссылка. Если без первого слеша - как локальная. Например: "//vk.com" выведит как есть, а vk.com выведет как /vk.com.'),
        '#default_value' => variable_get('sliders_link_' . $i, ''),
      );
      // Сохранить текущий номер изображения с помощью скрытого поля формы
       $form['sliders_img_' . $i]['number'] = array('#type' => 'hidden', '#value' => $i);
           // Кнопка удаление изображения
       $form['sliders_img_' . $i]['delete'] = array(
        '#type' => 'submit',
        '#name' => 'delete_img_' . $i,
        '#value' => t('Удалить изображение'),
        '#submit' => array('_sliders_delete_img'), //
       );
    }//вывод изображений
  }//Условия, чтобы изображений было большей нуля




  $form['submit']  = array(
    '#type' => 'submit',
    '#name' => 'sliders_save',
    '#value' => t('Сохранить все изменения'),
    '#submit' => array('_slider_settings_form_submit'),
  );

  $form['#attributes'] = array('id' => 'slider_settings_form');
  //$form['#validate'][] = '_slider_settings_form_validate';

  return $form;
}


/**
 * Функция добавления изображения в слайдер
 */
function _sliders_add_img($form, &$form_state) {

  $sliders_img_count = variable_get('sliders_img_count', 0);
  // Увеличиваем количество изображений
  $sliders_img_count++; 
  // Пересохраняем количество изображений
  variable_set('sliders_img_count', $sliders_img_count);
  // Обновляем форму
  $form_state['rebuild'] = TRUE;
  drupal_set_message( t('Изображение добавлено') );

}

//hook_form
/**
 * Реализуем удаление изображения
 */

function _sliders_delete_img($form, &$form_state) {
  $values = $form_state['values'];
  // Узнаём текущий номер изображения использую работу со строками
  $number = str_replace('edit-sliders-img-', '', $values['general__active_tab']);

  $sliders_img_count = variable_get('sliders_img_count', 0);
  // Сохранить  fid удаляемого изображения
  $img_fid = variable_get('slidres_img_' . $number, NULL);
  // Передалываем структуру
  for ($i = $number; $i < $sliders_img_count; $i++) { 
    $next = $i + 1;
    // Получаем следующиее значение
    $next_img_fid = variable_get('slidres_img_' . $next, NULL); 
    $next_description = variable_get('sliders_descriptoin_' . $next, '');
    $next_link = variable_get('sliders_link_' . $next, '');
    // Заменя текущего значения следующим
    variable_set('slidres_img_' . $i, $next_img_fid); 
    variable_set('sliders_descriptoin_' . $i, $next_description);
    variable_set('sliders_link_' . $i, $next_link);
  }
  //Удаление последнего элемента, который совпадает с предпоследним
  // Удалим файл
  $file = file_load(  $img_fid );
  file_delete( $file );
  // Удалим все настройки из таблицы переменных variable
  variable_del('slidres_img_' . $sliders_img_count);
  variable_del('sliders_descriptoin_' . $sliders_img_count);
  variable_del('sliders_link_' . $sliders_img_count);

  //Уменьшаем количество изображений 
  $sliders_img_count--;
  variable_set('sliders_img_count', $sliders_img_count);

  drupal_set_message( t('Выбранное изображение удалено') );
}

/**
 * Implementation of hook_settings_form_submit().
 */
function _slider_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  //drupal_set_message('<pre>' . print_r( $values, 1) . '</pre>');
  //drupal_set_message('<pre>' . print_r( $form_state, 1) . '</pre>');
  $sliders_img_count = variable_get('sliders_img_count', 0);

   for ($i = 1; $i <= $sliders_img_count; $i++) { 

      variable_set('slidres_img_' . $i, $values['slider_file_' . $i]); 
      variable_set('sliders_descriptoin_' . $i, $values['sliders_descriptoin_' . $i]);
      variable_set('sliders_link_' . $i, $values['sliders_link_' . $i]);
      // Сохранение изображения
      $fid = variable_get('slidres_img_' . $i, 0); 
      $file = file_load( $fid );
        // Если статус действительно "Временно", то...
        if ($file->status == 0) {
          // Устанавливаем нормальный статус.
          $file->status = FILE_STATUS_PERMANENT;
          // Сохраняем наш файл.
          file_save($file);
          file_usage_add($file, 'sliders', 'sliders-' . $i, $fid);
        }
   }
   variable_set('sliders_previews', $values['preview']);
   variable_set('sliders_img_style', $values['styles']);
   drupal_set_message( t('Изменения сохранились') );
}