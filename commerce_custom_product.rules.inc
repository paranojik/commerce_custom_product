<?php
/**
 * @file
 * Rules integration for Commerce Custom Product.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_custom_product_rules_condition_info() {
  $conditions['commerce_order_contains_customizable_products'] = array(
    'label' => t('Order contains customizable products'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order whose line items should be checked for customizable products.'),
      ),
    ),
    'group' => t('Commerce Custom Product'),
    'base' => 'commerce_custom_product_rules_order_contains_customizable_products',
  );
  return $conditions;
}

/**
 * Condition callback: checks if an order contains customizable products.
 */
function commerce_custom_product_rules_order_contains_customizable_products($order) {
  $extra_line_items = commerce_custom_product_order_get_existing_extra_line_items($order);
  return !empty($extra_line_items);
}
