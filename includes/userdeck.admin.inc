<?php
/**
 * @file
 * UserDeck module include file for admin pages.
 */

/**
 * Builds basic UserDeck Admin settings page.
 */
function userdeck_admin() {
  $form = array();

  $form['userdeck'] = array(
    '#type' => 'markup',
    '#markup' => '',
    '#prefix' => '<div id="userdeck-wrapper" class="wrap">',
    '#suffix' => '</div>',
  );

  $form['userdeck']['header'] = array(
    '#type' => 'markup',
    '#markup' => '
      <h2>UserDeck</h2>
      <p>
        <a href="http://userdeck.com?utm_source=drupal&utm_medium=link&utm_campaign=website" target="_blank">UserDeck</a>
        provides customer support software that embeds into your Drupal website.
      </p>',
  );

  if ($guides_key = variable_get('userdeck_guides_key', FALSE)) {
    $form['userdeck']['guides'] = array(
      '#type' => 'fieldset',
      '#title' => t('Guides'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['userdeck']['guides']['create_a_page'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create a Page'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $form['userdeck']['guides']['create_a_page']['create_a_page'] = array(
      '#type' => 'textfield',
      '#title' => t('Page Title'),
      '#size' => 25,
      '#maxlength' => 255,
      '#description' => t('Create a new page with the Guides shortcode.'),
    );

    $form['userdeck']['guides']['create_a_page']['create_a_page_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create Page'),
    );

    $res = db_query("SELECT nid, title FROM {node} WHERE type = 'page' AND status = 1");
    $nodes = $res->fetchAllKeyed();
    if ($nodes) {
      $form['userdeck']['guides']['add_to_page'] = array(
        '#type' => 'fieldset',
        '#title' => t('Add to Page'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
      );

      $form['userdeck']['guides']['add_to_page']['add_to_page'] = array(
        '#type' => 'select',
        '#title' => t('Page Title'),
        '#options' => $nodes,
        '#description' => t('Add the Guides shortcode to an existing page.'),
      );

      $form['userdeck']['guides']['add_to_page']['add_to_page_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add to Page'),
      );
    }

    $form['userdeck']['guides']['copy_shortcode'] = array(
      '#type' => 'fieldset',
      '#title' => t('Copy Shortcode'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $form['userdeck']['guides']['copy_shortcode']['copy_shortcode'] = array(
      '#type' => 'textfield',
      '#title' => t('Shortcode'),
      '#disabled' => 1,
      '#default_value' => userdeck_get_guides_shortcode(),
      '#description' => 'Copy and paste this code into the body of the page you would like to display Guides on.',
    );

  }
  else {

    if (variable_get('userdeck_guides_key_nonce', FALSE)) {
      variable_del('userdeck_guides_key_nonce');
    }

    $nonce = sha1('userdeck-guides-key-nonce-' . time());

    variable_set('userdeck_guides_key_nonce', $nonce);

    drupal_add_js(drupal_get_path('module', 'userdeck') . '/js/userdeck.js');
    drupal_add_js(array('module_settings_nonce' => $nonce), 'setting');

    $form['userdeck']['connect'] = array(
      '#type' => 'markup',
      '#markup' => '
        <p>
          An account is required to use the plugin. Don\'t have an account? You can create one for free.
        </p>
        <div id="button-connect">
          <h3>Connect to UserDeck</h3>
          <a href="javascript:void(0)" onclick="UserDeck.showConnect(\'login\')" class="button">Login</a>
          <span style = "margin: 0 10px; font-size: 16px; line-height: 42px;" >or</span >
          <a href="javascript:void(0)" onclick="UserDeck.showConnect(\'signup\')" class="button" >Signup</a>
        </div>
        <div id = "connect-frame" ></div>
      ',
    );

    $form['userdeck']['guides']['features'] = array(
      '#type' => 'markup',
      '#markup' => '
          <h2>Features</h2>
          <h4>Guides</h4>
          <p>
            A knowledge base widget that embeds inline to any page of your Drupal website.
          </p>
          <p>
            It inherits your theme\'s design and blends right in.
          </p>
          <p>
            You can embed a collection, category, or a single article instead of an entire knowledge base.
          </p>
          <p>
            Your users will save time by finding answers to common questions through self service.
          </p>
          <p>
            <a href="http://userdeck.com/guides?utm_source=drupal&utm_medium=link&utm_campaign=website" target="_blank">Learn more about Guides</a>
          </p>
        ',
      '#prefix' => '<div id="feature-wrapper">',
      '#suffix' => '</div>',
    );

    $form['userdeck']['guides']['style'] = array(
      '#type' => 'markup',
      '#markup' => "
        <style type = 'text/css' >
          #button-connect { margin: 40px 0; }
          #button-connect a { padding: 10px 20px; margin: 0;}
          #iframe-guides { display: none; box-shadow: 0 1px 1px rgba(0,0,0,.04); border: 1px solid #e5e5e5; padding: 2px; background: #fff; }
        </style>
      ",
    );
  }

  return $form;
}

/**
 * UserDeck Admin Settings form validation callback.
 */
function userdeck_admin_validate($form, &$form_state) {
  $clicked = $form_state['clicked_button'];
  $vals = $form_state['values'];

  if ($clicked['#value'] == 'Create Page') {
    if ($page_name = $vals['create_a_page']) {
      $res = db_query("SELECT nid, title FROM {node} WHERE type = 'page' AND status = 1 AND title = '$page_name'");
      $nodes = $res->fetchAllKeyed();
      if ($nodes) {
        form_set_error('create_a_page', 'A page with that name already exists.');
        return FALSE;
      }
    }
    else {
      form_set_error('create_a_page', 'You must enter a name for the new page.');
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * UserDeck Admin settings form submit callback.
 */
function userdeck_admin_submit($form, &$form_state) {
  $clicked = $form_state['clicked_button'];
  $vals = $form_state['values'];

  $nid = FALSE;

  switch ($clicked['#value']) {
    case 'Create Page' :
      $nid = userdeck_create_new_guides_page($vals['create_a_page']);
      if ($nid) {
        $node = node_load($nid);
        drupal_set_message("<a href='/node/$nid'>{$node->title}</a> created with Guides shortcode!");
      }
      break;
    case 'Add to Page':
      $nid = userdeck_add_guide_to_page($vals['add_to_page']);
      if ($nid) {
        $node = node_load($nid);
        drupal_set_message("Guides shortcode added to <a href='/node/$nid'>{$node->title}</a>!");
      }
      break;
    default :
      break;
  }

  if (!$nid) {
    form_set_error('guides', 'There was an error processing the form, please try again.');
  }

  return (bool) $nid;
}