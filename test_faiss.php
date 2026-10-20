<?php
/*
   +----------------------------------------------------------------------+
   | Copyright (c) 2026 Roo J Solutions Ltd.                              |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Leon <leon@roojs.com>                                        |
   +----------------------------------------------------------------------+
*/

if (!extension_loaded('faiss')) {
    fwrite(STDERR, "SKIP: faiss extension not loaded\n");
    exit(0);
}

$dim = 4;
$idx = FaissIndex::create($dim);
$vec = array(1.0, 0.0, 0.0, 0.0);
$idx->add($vec);
assert($idx->count() === 1, 'count after add');

$hits = $idx->searchWithIds($vec, 1, array(0));
assert(count($hits) === 1, 'one search hit');
assert($hits[0]['id'] === 0, 'label 0 for first vector');

$idx->add(array(0.0, 1.0, 0.0, 0.0));
$idx->add(array(0.0, 0.0, 1.0, 0.0));
assert($idx->count() === 3, 'count after three adds');

$restricted = $idx->searchWithIds($vec, 2, array(1, 2));
assert(count($restricted) >= 1, 'searchWithIds returns hits');
foreach ($restricted as $hit) {
    assert($hit['id'] === 1 || $hit['id'] === 2, 'searchWithIds only from id subset');
}
$empty = $idx->searchWithIds($vec, 1, array());
assert(count($empty) === 0, 'searchWithIds empty ids');

$path = sys_get_temp_dir() . '/php_faiss_test_' . getmypid() . '.faiss';
$idx->save($path);
$loaded = FaissIndex::load($path);
assert($loaded->count() === 3, 'loaded count');
@unlink($path);

fwrite(STDERR, "test_faiss.php OK\n");
