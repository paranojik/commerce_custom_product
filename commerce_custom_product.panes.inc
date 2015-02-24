<?php
/**
 * @file
 * Contains all pane method callbacks.
 */

/**
 * Commerce checkout method callback for CALLBACK_settings_form().
 */
function commerce_custom_product_customizing_pane_settings_form($checkout_pane) {
  return array(
    'commerce_custom_product_customizing_pane_description' => array(
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#default_value' => variable_get('commerce_custom_product_customizing_pane_description', ''),
    ),
    'commerce_custom_product_customizing_pane_update_button' => array(
      '#type' => 'checkbox',
      '#title' => t('Add update button'),
      '#description' => t('Add update button on the checkout pane.'),
      '#default_value' => variable_get('commerce_custom_product_customizing_pane_update_button', TRUE),
    ),
    'commerce_custom_product_customizing_skip' => array(
      '#type' => 'checkbox',
      '#title' => t('Skip customization if no customizable products are available'),
      '#description' => t('Continues to the next checkout step if no customizable products are available.'),
      '#default_value' => variable_get('commerce_custom_product_customizing_skip', TRUE),
    ),
  );
}

/**
 * Commerce checkout method callback for CALLBACK_checkout_form().
 */
function commerce_custom_product_customizing_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  $form = array();

  $extra_line_item_data = commerce_custom_product_order_get_existing_extra_line_items($order);

  // Add description.
  $description = variable_get('commerce_custom_product_customizing_pane_description', '');
  if ($description) {
    $form['description'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="postauto-commerce-customizing-pane-description">',
      '#markup' => check_plain($description),
      '#suffix' => '</div>',
      '#weight' => -10,
    );
  }
  $form['extra_line_items']['#tree'] = TRUE;
  foreach ($extra_line_item_data as $parent_line_item_id => $data) {
    $form['extra_line_items'][$parent_line_item_id] = array(
      '#type' => 'fieldset',
      '#title' => $data['parent_line_item']->line_item_label,
      '#description' => t('Customize %line_item', array('%line_item' => $data['parent_line_item']->line_item_label)),
    );
    foreach ($data['extra_line_items'] as $i => $extra_line_item) {
      $parents = array(
        'extra_line_items',
        $parent_line_item_id,
        'line_items',
        $extra_line_item->data['context']['extra_line_item_index'],
        $i,
      );
      $form['extra_line_items'][$parent_line_item_id]['line_items'][$extra_line_item->data['context']['extra_line_item_index']][$i] = commerce_custom_product_extra_line_item_form_element($extra_line_item, $form_state, $parents);
      $form['extra_line_items'][$parent_line_item_id]['line_items'][$extra_line_item->data['context']['extra_line_item_index']][$i]['line_item_id'] = array(
        '#type' => 'value',
        '#value' => $extra_line_item->line_item_id,
      );
    }
  }
  if (variable_get('commerce_custom_product_customizing_pane_update_button', TRUE)) {
    $form['update'] = array(
      '#type' => 'submit',
      '#value' => t('Update customization'),
      '#submit' => array('commerce_custom_product_customizing_pane_checkout_form_update'),
    );
    // Ensure this file is available.
    form_load_include($form_state, 'inc', 'commerce_custom_product', 'commerce_custom_product.panes');
  }

  return $form;
}

/**
 * Commerce checkout method callback for CALLBACK_checkout_validate().
 */
function commerce_custom_product_customizing_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  return TRUE;
}

/**
 * Submit handler for the update button.
 */
function commerce_custom_product_customizing_pane_checkout_form_update($form, &$form_state) {
  commerce_custom_product_customizing_pane_checkout_form_submit($form, $form_state, commerce_checkout_pane_load('customizing_pane'), $form_state['order']);
  $form_state['rebuild'] = TRUE;
}

/**
 * Commerce checkout method callback for CALLBACK_checkout_form_submit().
 */
function commerce_custom_product_customizing_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  foreach ($form_state['values']['extra_line_items'] as $parent_line_item_id => $extra_line_items) {
    foreach ($extra_line_items['line_items'] as $extra_line_item_index => $line_items) {
      foreach ($line_items as $i => $line_item_field_values) {
        $line_item_id = $line_item_field_values['line_item_id'];
        $line_item_form_element = $form[$checkout_pane['pane_id']]['extra_line_items'][$parent_line_item_id]['line_items'][$extra_line_item_index][$i];
        // Remove line item field values the user didn't have access to modify.
        foreach ($line_item_field_values as $field_name => $value) {
          // Note that we're checking the Commerce Cart settings that we
          // inserted into this form element array back when we built the form.
          // This means a module wanting to alter a line item field widget to be
          // available must update both its form element's #access value and the
          // field_access value of the #commerce_cart_settings array.
          if (empty($line_item_form_element[$field_name]['#commerce_cart_settings']['field_access'])) {
            unset($line_item_field_values[$field_name]);
          }
        }
        // Unset the line item field values array if it is now empty.
        if (!empty($line_item_field_values)) {
          $line_item = commerce_line_item_load($line_item_id);
          // Add field data to the line item.
          field_attach_submit('commerce_line_item', $line_item, $line_item_form_element, $form_state);
          commerce_line_item_save($line_item);
        }
      }
    }
  }
}
