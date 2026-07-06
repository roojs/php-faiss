<?php
if (!extension_loaded('faiss')) {
    fwrite(STDERR, "SKIP: faiss extension not loaded\n");
    exit(0);
}

$dim = 4;
$idx = FaissIndex::create($dim);
$vec = array(1.0, 0.0, 0.0, 0.0);
$idx->add($vec);
assert($idx->count() === 1, 'count after add');

$hits = $idx->search($vec, 1);
assert(count($hits) === 1, 'one search hit');
assert($hits[0]['id'] === 0, 'label 0 for first vector');

$path = sys_get_temp_dir() . '/php_faiss_test_' . getmypid() . '.faiss';
$idx->save($path);
$loaded = FaissIndex::load($path);
assert($loaded->count() === 1, 'loaded count');
@unlink($path);

fwrite(STDERR, "test_faiss.php OK\n");
