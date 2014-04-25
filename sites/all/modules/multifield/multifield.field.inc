<?php

/**
 * @file
 * Field integration for the Multifield module.
 */

// Trying to keep track of all the relevant field hooks that need to be run.
//  field_attach_delete()
//  field_attach_form()
//  field_attach_form_validate()
//  field_attach_insert()
//  field_attach_load()
//  field_attach_load_revision()
//  field_attach_prepare_view()
//  field_attach_preprocess()
//  field_attach_presave()
//  field_attach_submit()
//  field_attach_update()
//X field_attach_validate()
//  field_attach_view()

/**
 * Implements hook_field_info().
 */
function multifield_field_info() {
  $info = array();
  $multifields = multifield_load_all();
  foreach ($multifields as $machine_name => $multifield) {
    // Only show this multifield as an option to create if it has subfields.
    if (multifield_type_has_subfields($machine_name)) {
      $info[$machine_name] = array(
        'label' => $multifield->label,
        'description' => $multifield->description,
        'default_widget' => 'multifield_default',
        'default_formatter' => 'multifield_default',
      );
    }
  }
  return $info;
}

/**
 * Implements hook_field_load().
 */
function multifield_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  foreach (multifield_type_get_subfields($field['type']) as $field_name) {
    $subfield = field_info_field($field_name);
    foreach (array_keys($subfield['columns']) as $column) {
      foreach ($entities as $id => $entity) {
        array_walk($items[$id], 'multifield_item_unserialize', $field);
      }
    }

    // Once the subfields have been imploded into a proper structure, we need
    // to filter out 'empty' values.
    foreach (array_keys($entities) as $id) {
      foreach ($items[$id] as $delta => $item) {
        if (!empty($item[$field_name][LANGUAGE_NONE])) {
          $items[$id][$delta][$field_name][LANGUAGE_NONE] = _field_filter_items($subfield, $item[$field_name][LANGUAGE_NONE]);
        }
      }
    }
  }

  $pseudo_entities = array();
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => &$item) {
      $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item, array('delta' => $delta, 'parent_id' => $id));
      $pseudo_entities[$pseudo_entity->id] = $pseudo_entity;
    }
  }
  _multifield_field_attach_load('multifield', $pseudo_entities, $age);

  // Store the loaded multifield entities in the entity cache.
  entity_get_controller('multifield')->cacheSet($pseudo_entities);

  foreach ($pseudo_entities as $pseudo_entity) {
    $items[$pseudo_entity->parent_id][$pseudo_entity->delta] = _multifield_field_entity_to_item($pseudo_entity);
  }
}

function _multifield_field_attach_load($entity_type, $entities, $age = FIELD_LOAD_CURRENT, $options = array()) {
  // Invoke field-type module's hook_field_load().
  $null = NULL;
  _field_invoke_multiple('load', $entity_type, $entities, $age, $null, $options);

  // Invoke hook_field_attach_load(): let other modules act on loading the
  // entity.
  module_invoke_all('field_attach_load', $entity_type, $entities, $age, $options);
}

/**
 * Implements hook_field_access().
 *
 * @todo Investigate improving performance of this function.
 */
function multifield_field_access($op, $field, $entity_type, $entity, $account) {
  // We should return FALSE if all the subfields' field_access() checks all
  // return FALSE.
  if ($field['module'] == 'multifield') {
    $subfield_names = multifield_type_get_subfields($field['type']);
    $subfield_access = array();

    if (!empty($entity) && $items = field_get_items($entity_type, $entity, $field['field_name'])) {
      foreach ($items as $delta => $item) {
        $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
        foreach ($subfield_names as $subfield_name) {
          $subfield = field_info_field($subfield_name);
          $subfield_access[$subfield_name . ':' . $delta] = field_access($op, $subfield, 'multifield', $pseudo_entity, $account);
        }
      }
    }
    else {
      foreach ($subfield_names as $subfield_name) {
        $subfield = field_info_field($subfield_name);
        $subfield_access[$subfield_name] = field_access($op, $subfield, 'multifield', NULL, $account);
      }
    }

    // If all of the subfields returned FALSE, then this should return FALSE.
    if (!array_filter($subfield_access)) {
      return FALSE;
    }
  }
}

/**
 * Implements hook_field_validate().
 */
function multifield_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
    //_multifield_field_attach_form_validate('multifield', $pseudo_entity);
    //$items[$delta] = _multifield_field_entity_to_item($pseudo_entity);
  }
}

function _multifield_field_attach_form_validate($entity_type, $entity) {
  // Validate $options since this is a new parameter added after Drupal 7 was
  // released.
  $options = is_array($options) ? $options : array();

  // Extract field values from submitted values.
  //_field_invoke_default('extract_form_values', $entity_type, $entity, $form, $form_state);

  // Perform field_level validation.
  try {
    field_attach_validate($entity_type, $entity, $options);
  }
  catch (FieldValidationException $e) {
    // Pass field-level validation errors back to widgets for accurate error
    // flagging.
    foreach ($e->errors as $field_name => $field_errors) {
      foreach ($field_errors as $langcode => $errors) {
        $field_state = field_form_get_state($form['#parents'], $field_name, $langcode, $form_state);
        $field_state['errors'] = $errors;
        field_form_set_state($form['#parents'], $field_name, $langcode, $form_state, $field_state);
      }
    }
    _field_invoke_default('form_errors', $entity_type, $entity, $form, $form_state, $options);
  }
}

/**
 * Implements hook_field_presave().
 */
function multifield_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Run each sub-field through hook_field_presave().
  foreach ($items as $delta => $item) {
    if (empty($item['id'])) {
      $item['id'] = multifield_get_next_id();
    }
    $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
    // field_attach_presave() is safe to call directly, yay!
    field_attach_presave('multifield', $pseudo_entity);
    $items[$delta] = _multifield_field_entity_to_item($pseudo_entity);
  }

  // Serialize the multifield values into separate columns for saving into the
  // field table.
  array_walk($items, 'multifield_item_serialize', $field);
}

function multifield_field_attach_submit($entity_type, $entity, $form, $form_state) {
  list(, , $bundle) = entity_extract_ids($entity_type, $entity);
  $multifields = multifield_get_fields();
  $instances = array_intersect_key(field_info_instances($entity_type, $bundle), $multifields);
  foreach (array_keys($instances) as $field_name) {
    if (!empty($entity->{$field_name})) {
      foreach ($entity->{$field_name} as $langcode => $items) {
        foreach ($items as $delta => $item) {
          $pseudo_entity = _multifield_field_item_to_entity($multifields[$field_name], $item);
          _multifield_field_attach_submit('multifield', $pseudo_entity, $form, $form_state);
          $entity->{$field_name}[$langcode][$delta] = _multifield_field_entity_to_item($pseudo_entity);
        }
      }
    }
  }
}

function _multifield_field_attach_submit($entity_type, $entity, $form, &$form_state, $options = array()) {
  _field_invoke_default('submit', $entity_type, $entity, $form, $form_state, $options);

  // Let other modules act on submitting the entity.
  // Avoid module_invoke_all() to let $form_state be taken by reference.
  foreach (module_implements('field_attach_submit') as $module) {
    $function = $module . '_field_attach_submit';
    $function($entity_type, $entity, $form, $form_state);
  }
}

/**
 * Implements hook_field_is_empty().
 */
function multifield_field_is_empty($item, $field) {
  $instances = field_info_instances('multifield', $field['type']);
  foreach ($instances as $field_name => $instance) {
    if (!empty($item[$field_name])) {
      $subfield = field_info_field($field_name);
      foreach ($item[$field_name] as $langcode => $subfield_items) {
        $item[$field_name][$langcode] = _field_filter_items($subfield, $subfield_items);
      }
      $item[$field_name] = array_filter($item[$field_name]);
    }
  }
  return !array_filter(array_intersect_key($item, $instances));
}

/**
 * Implements hook_field_insert().
 */
function multifield_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  //array_walk($items, 'multifield_item_unserialize', $field);

  // Run each sub-field through hook_field_insert().
  foreach ($items as $delta => $item) {
    $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
    _multifield_field_attach_insert('multifield', $pseudo_entity);
    $items[$delta] = _multifield_field_entity_to_item($pseudo_entity);
  }

  // Because this is invoked right prior to field storage writing, we need to
  // re-serialize the field values.
  array_walk($items, 'multifield_item_serialize', $field);

  multifield_update_maximum_id($items);
}

function _multifield_field_attach_insert($entity_type, $entity) {
  _field_invoke_default('insert', $entity_type, $entity);
  _field_invoke('insert', $entity_type, $entity);

  // Let any module insert field data before the storage engine, accumulating
  // saved fields along the way.
  //$skip_fields = array();
  //foreach (module_implements('field_storage_pre_insert') as $module) {
  //  $function = $module . '_field_storage_pre_insert';
  //  $function($entity_type, $entity, $skip_fields);
  //}

  // Let other modules act on inserting the entity.
  module_invoke_all('field_attach_insert', $entity_type, $entity);
}

/**
 * Implements hook_field_update().
 */
function multifield_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  //array_walk($items, 'multifield_item_unserialize', $field);

  // Run each sub-field through hook_field_update().
  foreach ($items as $delta => $item) {
    $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
    _multifield_field_attach_update('multifield', $pseudo_entity);
    $items[$delta] = _multifield_field_entity_to_item($pseudo_entity);
  }

  // Because this is invoked right prior to field storage writing, we need to
  // re-serialize the field values.
  array_walk($items, 'multifield_item_serialize', $field);

  multifield_update_maximum_id($items);
}

function _multifield_field_attach_update($entity_type, $entity) {
  _field_invoke('update', $entity_type, $entity);

  // Let any module update field data before the storage engine, accumulating
  // saved fields along the way.
  //$skip_fields = array();
  //foreach (module_implements('field_storage_pre_update') as $module) {
  //  $function = $module . '_field_storage_pre_update';
  //  $function($entity_type, $entity, $skip_fields);
  //}

  // Let other modules act on updating the entity.
  module_invoke_all('field_attach_update', $entity_type, $entity);
}

/**
 * Implements hook_field_delete().
 */
function multifield_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Run each sub-field through hook_field_delete().
  $ids = array();
  foreach ($items as $delta => $item) {
    $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item);
    $ids[] = $pseudo_entity->id;
    _multifield_field_attach_delete('multifield', $pseudo_entity);
    $items[$delta] = _multifield_field_entity_to_item($pseudo_entity);
  }

  entity_get_controller('multifield')->resetCache($ids);
}

function _multifield_field_attach_delete($entity_type, $entity) {
  _field_invoke('delete', $entity_type, $entity);

  // Let other modules act on deleting the entity.
  module_invoke_all('field_attach_delete', $entity_type, $entity);
}

/**
 * Implements hook_field_widget_info().
 */
function multifield_field_widget_info() {
  $info = array();
  $multifields = multifield_load_all();

  if (!empty($multifields)) {
    $info['multifield_default'] = array(
      'label' => t('Default'),
      'field types' => array_keys($multifields),
    );
  }

  return $info;
}

/**
 * Implements hook_field_widget_form().
 */
function multifield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $item = isset($items[$delta]) ? $items[$delta] : array();

  $element['#parents'] = array_merge($element['#field_parents'], array($element['#field_name'], $element['#language'], $element['#delta']));

  // Force the ID of the pseudo entity to be NULL, to prevent modules like
  // entity reference from trying to use it.
  $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item, array('id' => NULL));

  // Rather than calling field_attach_form() here, we have to limit these
  // sub-field widgets to only one cardinality value. So manually invoke
  // field_default_form() for each one.
  foreach (field_info_instances('multifield', $field['type']) as $field_name => $instance) {
    $subfield = field_info_field($field_name);
    $subfield['cardinality'] = 1;
    // If a subfield is required, but this is not the first delta, or this
    // widget it being used in the default value form for the multifield, then
    // disable the subfield's requirement flag.
    if ($instance['required'] && ($delta > 0 || empty($element['#entity']))) {
      $instance['required'] = 0;
    }
    $subitems = isset($pseudo_entity->{$field_name}[LANGUAGE_NONE]) ? $pseudo_entity->{$field_name}[LANGUAGE_NONE] : array();
    $element += field_default_form('multifield', $pseudo_entity, $subfield, $instance, LANGUAGE_NONE, $subitems, $element, $form_state, 0);
  }
  //field_attach_form('multifield', $pseudo_entity, $element, $form_state, $langcode);

  // If this multifield itself has a cardinality of one value, and this is not
  // being used for the field default value form, then set the wrapping element
  // to be a fieldset for display/grouping purposes.
  if ($field['cardinality'] == 1 && !empty($element['#entity'])) {
    $element['#type'] = 'fieldset';
  }

  $element['id'] = array(
    '#type' => 'value',
    '#value' => !empty($item['id']) ? $item['id'] : NULL,
  );

  $element['actions'] = array(
    '#type' => 'actions',
    //'#attached' => array(
    //  'css' => array(
    //    drupal_get_path('module', 'multifield') . '/multifield.field.css' => array(),
    //  ),
    //),
  );
  $element['actions']['remove_button'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
    '#name' => implode('_', $element['#parents']) . '_remove_button',
    '#delta' => $delta,
    '#submit' => array('multifield_field_widget_remove_item_submit'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'path' => 'multifield/field-remove-item/ajax',
      'effect' => 'fade',
    ),
    '#attributes' => array(
      'class' => array('remove-button', 'delta-' . $delta),
    ),
    '#access' => $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED,
  );

  //$form['#validate'][] = 'multifield_field_widget_validate';
  //$form['#multifields'][] = $element['#parents'];

  return $element;
}

function multifield_field_widget_validate($element, &$form_state) {
  //dpm($element, __FUNCTION__);
  //dpm($form_state, __FUNCTION__);
}

/**
 * Page callback to handle AJAX for removing a multifield item.
 *
 * Copied from field_collection_remove_js().
 *
 * This is a direct page callback. The actual job of deleting the item is
 * done in the submit handler for the button, so all we really need to
 * do is process the form and then generate output. We generate this
 * output by doing a replace command on the id of the entire form element.
 */
function multifield_field_widget_remove_item_ajax() {
  require_once DRUPAL_ROOT . '/includes/form.inc';

  // drupal_html_id() very helpfully ensures that all html IDS are unique
  // on a page. Unfortunately what it doesn't realize is that the IDs
  // we are generating are going to replace IDs that already exist, so
  // this actually works against us.
  if (isset($_POST['ajax_html_ids'])) {
    unset($_POST['ajax_html_ids']);
  }

  list($form, $form_state) = ajax_get_form();
  drupal_process_form($form['#form_id'], $form, $form_state);

  // Get the information on what we're removing.
  $button = $form_state['triggering_element'];
  // Go two levels up in the form, to the whole widget.
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -4));
  // Now send back the proper AJAX command to replace it.
  $return = array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace('#' . $element['#id'], drupal_render($element))
    ),
  );

  // Because we're doing this ourselves, messages aren't automatic. We have
  // to add them.
  $messages = theme('status_messages');
  if ($messages) {
    $return['#commands'][] = ajax_command_prepend('#' . $element['#id'], $messages);
  }

  return $return;
}

/**
 * Submit callback to remove an item from the field UI multiple wrapper.
 *
 * Copied from field_collection_remove_submit()
 *
 * When a remove button is submitted, we need to find the item that it
 * referenced and delete it. Since field UI has the deltas as a straight
 * unbroken array key, we have to renumber everything down. Since we do this
 * we *also* need to move all the deltas around in the $form_state['values']
 * and $form_state['input'] so that user changed values follow. This is a bit
 * of a complicated process.
 */
function multifield_field_widget_remove_item_submit($form, &$form_state) {
  $button = $form_state['triggering_element'];
  $delta = $button['#delta'];

  // Where in the form we'll find the parent element.
  $address = array_slice($button['#array_parents'], 0, -3);

  // Go one level up in the form, to the widgets container.
  $parent_element = drupal_array_get_nested_value($form, $address);
  $field_name = $parent_element['#field_name'];
  $langcode = $parent_element['#language'];
  $parents = $parent_element['#field_parents'];

  $field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);

  // Go ahead and renumber everything from our delta to the last
  // item down one. This will overwrite the item being removed.
  for ($i = $delta; $i <= $field_state['items_count']; $i++) {
    $old_element_address = array_merge($address, array($i + 1));
    $new_element_address = array_merge($address, array($i));

    $moving_element = drupal_array_get_nested_value($form, $old_element_address);
    $moving_element_value = drupal_array_get_nested_value($form_state['values'], $old_element_address);
    $moving_element_input = drupal_array_get_nested_value($form_state['input'], $old_element_address);

    // Tell the element where it's being moved to.
    $moving_element['#parents'] = $new_element_address;

    // Move the element around.
    form_set_value($moving_element, $moving_element_value, $form_state);
    drupal_array_set_nested_value($form_state['input'], $moving_element['#parents'], $moving_element_input);
  }

  // Then remove the last item. But we must not go negative.
  if ($field_state['items_count'] > 0) {
    $field_state['items_count']--;
  }

  // Fix the weights. Field UI lets the weights be in a range of
  // (-1 * item_count) to (item_count). This means that when we remove one,
  // the range shrinks; weights outside of that range then get set to
  // the first item in the select by the browser, floating them to the top.
  // We use a brute force method because we lost weights on both ends
  // and if the user has moved things around, we have to cascade because
  // if I have items weight weights 3 and 4, and I change 4 to 3 but leave
  // the 3, the order of the two 3s now is undefined and may not match what
  // the user had selected.
  $input = drupal_array_get_nested_value($form_state['input'], $address);
  // Sort by weight
  uasort($input, '_field_sort_items_helper');

  // Reweight everything in the correct order.
  $weight = -1 * $field_state['items_count'];
  foreach ($input as $key => $item) {
    if ($item) {
      $input[$key]['_weight'] = $weight++;
    }
  }

  drupal_array_set_nested_value($form_state['input'], $address, $input);
  field_form_set_state($parents, $field_name, $langcode, $form_state, $field_state);

  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_field_formatter_view().
 */
function multifield_field_formatter_info() {
  $info = array();
  $multifields = multifield_load_all();

  if (!empty($multifields)) {
    $info['multifield_default'] = array(
      'label' => t('Default'),
      'field types' => array_keys($multifields),
      'settings' => array(
        'view_mode' => 'default',
      ),
    );
  }

  return $info;
}

/**
 * Implements hook_field_formatter_settings().
 */
function multifield_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $entity_info = entity_get_info('multifield');
  $options = array();
  foreach ($entity_info['view modes'] as $view_mode_name => $view_mode) {
    $options[$view_mode_name] = $view_mode['label'];
  }

  $element['view_mode'] = array(
    '#type' => 'select',
    '#title' => t('View mode'),
    '#options' => array('default' => t('Default')) + $options,
    '#default_value' => $settings['view_mode'],
    '#required' => TRUE,
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function multifield_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $entity_info = entity_get_info('multifield');
  $view_mode_label = $settings['view_mode'] == 'default' ? t('Default') : $entity_info['view modes'][$settings['view_mode']]['label'];

  $summary = t('View mode: @view-mode', array('@view-mode' => $view_mode_label));

  return $summary;
}

/**
 * Implements hook_field_prepare_view().
 */
function multifield_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $display = NULL) {
  // When this hook is invoked using field_attach_prepare_view(), the view mode
  // being used to render the entities is not passed through here. So we need
  // a hack to retrieve it. When this hook is invoked using field_view_field(),
  // the display settings are passed into the last parameter.
  if (!isset($display)) {
    $backtrace = debug_backtrace();
    foreach ($backtrace as $caller) {
      if ($caller['function'] == 'field_attach_prepare_view') {
        $display = $caller['args'][2];
      }
    }
    if (!isset($display)) {
      throw new Exception("Unable to determine the view mode being used to render the parent entity of the multifield.");
    }
  }

  $view_mode_pseudo_entities = array();

  foreach ($entities as $id => $entity) {
    $instance_display = is_array($display) ? $display : field_get_display($instances[$id], $display, $entity);
    if (!$instance_display['type'] !== 'hidden') {
      $view_mode = !empty($display['settings']['view_mode']) ? $display['settings']['view_mode'] : 'default';
      foreach ($items[$id] as $delta => $item) {
        $pseudo_entity = _multifield_field_item_to_entity($field['type'], $item, array('delta' => $delta, 'parent_id' => $id));
        $view_mode_pseudo_entities[$view_mode][$pseudo_entity->id] = $pseudo_entity;
      }
    }
  }

  foreach ($view_mode_pseudo_entities as $view_mode => $pseudo_entities) {
    field_attach_prepare_view('multifield', $pseudo_entities, $view_mode, $langcode);
    foreach ($pseudo_entities as $pseudo_entity) {
      $items[$pseudo_entity->parent_id][$pseudo_entity->delta] = _multifield_field_entity_to_item($pseudo_entity);
      $pseudo_entity->id = NULL;
      $items[$pseudo_entity->parent_id][$pseudo_entity->delta]['#pseudo_entity'] = $pseudo_entity;
    }
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function multifield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];

  foreach ($items as $delta => $item) {
    $element[$delta] = field_attach_view('multifield', $item['#pseudo_entity'], $settings['view_mode'], $langcode);
  }

  return $element;
}
