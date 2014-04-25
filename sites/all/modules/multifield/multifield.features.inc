<?php
/**
 * Implements hook_features_api_alter().
 */
function multifield_features_api_alter(&$components) {
    // Multifield components must be handled before all field
    $keys = array_keys($components);
    $field_index = array_search('field', $keys);
    $field_base_index = array_search('field_base', $keys);
    $field_instance_index = array_search('field_instance', $keys);
    $new_index = min($field_index, $field_base_index, $field_instance_index);
    $current_index = array_search('multifield', $keys);
    $multifield = array_slice($components, $current_index, 1);
    $temp_components = $components;
    $components = array_slice($temp_components, 0, $new_index, TRUE)
        + $multifield
        + array_slice($temp_components, $new_index, NULL, TRUE);
  }

/**
 * Implements hook_field_default_fields_alter().
 */
function multifield_field_default_fields_alter(&$fields) {
    features_include_defaults('multifield');
    multifield_cache_clear();
    $fields_copy = $fields;
    foreach ($fields_copy as $key => $field) {
        $field_base = $field['field_config'];
        // Do not create multifields if they don't exist yet.
        if ($field_base['module'] == 'multifield' && (!multifield_load($field_base['type']) || !field_info_field_types($field_base['type']))) {
            unset($fields[$key]);
    }
  }
}

/**
 * Implements hook_field_default_field_bases_alter().
 */
function multifield_field_default_field_bases_alter(&$field_bases) {
    features_include_defaults('multifield');
    multifield_cache_clear();
    $field_bases_copy = $field_bases;
    foreach ($field_bases_copy as $key => $field_base) {
        // Do not create multifields if they don't exist yet.
        if ($field_base['module'] == 'multifield' && (!multifield_load($field_base['type']) || !field_info_field_types($field_base['type']))) {
            unset($field_bases[$key]);
    }
  }
}

/**
 * Implements of hook_features_rebuild().
 */
function multifield_features_rebuild($module) {
    features_include_defaults('multifield');
    multifield_cache_clear();
    // Rebuild all this modules base fields & instances, as long as they are not
    // a multifield field type. This way, all multifield subfields will be created
    // and we won't get the "Attempt to create a field of unknown type" exception.

    // Invoke hooks for consistency.
    module_invoke($module, 'pre_features_rebuild', 'field');
    _multifield_rebuild_field($module);
    module_invoke($module, 'post_features_rebuild', 'field');

    module_invoke($module, 'pre_features_rebuild', 'field_base');
    _multifield_rebuild_field_base($module);
    module_invoke($module, 'post_features_rebuild', 'field_base');

    module_invoke($module, 'pre_features_rebuild', 'field_instance');
    _multifield_rebuild_field_instance($module);
    module_invoke($module, 'post_features_rebuild', 'field_instance');

    multifield_cache_clear();
  }

/**
 * Implements hook_features_pipe_COMPONENT_alter().
 */
function multifield_features_pipe_multifield_alter(&$pipe, $data, $export) {
    foreach ($data as $multifield) {
        $subfields = multifield_type_get_subfields($multifield);
        foreach ($subfields as $subfield) {
            $pipe['field_instance'][] = "multifield-{$multifield}-{$subfield}";
          }
  }
}

/**
 * Implements hook_features_pipe_COMPONENT_alter().
 */
function multifield_features_pipe_field_base_alter(&$pipe, $data, $export) {
    foreach ($data as $field_name) {
        $info = field_info_field($field_name);
        if ($info && multifield_load($info['type'])) {
            $pipe['multifield'][] = $info['type'];
          }
  }
}

function _multifield_rebuild_field($module) {
    if ($fields = features_get_default('field', $module)) {
        field_info_cache_clear();

        // Load all the existing fields and instance up-front so that we don't
        // have to rebuild the cache all the time.
        $existing_fields = field_info_fields();
        $existing_instances = field_info_instances();

        foreach ($fields as $field) {
            // Create or update field.
            $field_config = $field['field_config'];
            if (!multifield_load($field['type'])) {
                if (isset($existing_fields[$field_config['field_name']])) {
                    $existing_field = $existing_fields[$field_config['field_name']];
                    if ($field_config + $existing_field !== $existing_field) {
                        try {
                            field_update_field($field_config);
                          }
            catch (FieldException $e) {
                            watchdog('features', 'Attempt to update field %label failed: %message', array('%label' => $field_config['field_name'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
                          }
          }
        }
        else {
                    try {
                        field_create_field($field_config);
                      }
          catch (FieldException $e) {
                        watchdog('features', 'Attempt to create field %label failed: %message', array('%label' => $field_config['field_name'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
                      }
          $existing_fields[$field_config['field_name']] = $field_config;
        }

        // Create or update field instance.
        $field_instance = $field['field_instance'];
        if (isset($existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']])) {
                    $existing_instance = $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']];
                    if ($field_instance + $existing_instance !== $existing_instance) {
                        field_update_instance($field_instance);
                      }
        }
        else {
                    field_create_instance($field_instance);
                    $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']] = $field_instance;
                  }
      }
    }

    if ($fields) {
            variable_set('menu_rebuild_needed', TRUE);
          }
  }
}

function _multifield_rebuild_field_base($module) {
    if ($fields = features_get_default('field_base', $module)) {
        field_info_cache_clear();

        // Load all the existing field bases up-front so that we don't
        // have to rebuild the cache all the time.
        $existing_fields = field_info_fields();

        foreach ($fields as $field) {
            if (!multifield_load($field['type'])) {
                // Create or update field.
                if (isset($existing_fields[$field['field_name']])) {
                    $existing_field = $existing_fields[$field['field_name']];
                    if ($field + $existing_field !== $existing_field) {
                        field_update_field($field);
                      }
        }
        else {
                    field_create_field($field);
                    $existing_fields[$field['field_name']] = $field;
                  }
        variable_set('menu_rebuild_needed', TRUE);
      }
    }
  }
}

function _multifield_rebuild_field_instance($module) {
    if ($instances = features_get_default('field_instance', $module)) {
        field_info_cache_clear();

        // Load all the existing instances up-front so that we don't
        // have to rebuild the cache all the time.
        $existing_instances = field_info_instances();

        foreach ($instances as $field_instance) {
            $field_info = field_info_field($field_instance['field_name']);
            // If the field base information does not exist yet, cancel out.
            if (!$field_info || multifield_load($field_info['type'])) {
                continue;
      }

      // Create or update field instance.
      if (isset($existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']])) {
                $existing_instance = $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']];
                if ($field_instance + $existing_instance !== $existing_instance) {
                    try {
                        field_update_instance($field_instance);
                      }
          catch (FieldException $e) {
                        watchdog('features', 'Attempt to update field instance %label (in %entity entity type %bundle bundle) failed: %message', array('%label' => $field_instance['field_name'], '%entity' => $field_instance['entity_type'], '%bundle' => $field_instance['bundle'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
                      }
        }
      }
      else {
                try {
                    field_create_instance($field_instance);
                  }
        catch (FieldException $e) {
                    watchdog('features', 'Attempt to create field instance %label (in %entity entity type %bundle bundle) failed: %message', array('%label' => $field_instance['field_name'], '%entity' => $field_instance['entity_type'], '%bundle' => $field_instance['bundle'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
                  }
        $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']] = $field_instance;
      }
    }

    if ($instances) {
            variable_set('menu_rebuild_needed', TRUE);
          }
  }
}