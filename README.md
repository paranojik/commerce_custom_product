# Commerce Custom Product

## Features

The module provides the new field type "Line item type reference" with this
field type you can define which additional line item types belong to a module.  
When adding a product to the cart the referenced line item types are taken in
account and if a referenced line item type exposes field to the end-users the
field can be edited by them.

Shipped with support for ctools content type "entity field". So you can use it
with Panels / Panelizer.

## Usage

  1. Create line item types containing the fields you'd like to provide for 
  product customization.
  2. Place a field of the type "Line item type reference" onto the product 
  display node you want to make customizable.
  3. Configure the "Add to cart" on your product display node display settings 
  to handle a "Extra line item types field".
  4. Edit the product display node and select the line item types you'd like to
  attach to this product when adding it into the cart.
  5. Profit!
