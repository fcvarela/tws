#!/usr/bin/env php -q
<?php
/*
 * 4 bytes face count
 * face component count
 * face components: 4 bytes vertex index, 4 bytes normal index, 4 bytes texcoord index
 */

ini_set('memory_limit', '1024M');

// initialize our data
$data = array();

load_obj_file("future_space_station.obj", &$data);
pipe_data(&$data);

function load_obj_file($filename, $data) {
	// read file into array of lines (simul readline)
	fwrite(STDERR, "Opening $filename\n");
	$lines = file($filename);

	// parse each line
	foreach ($lines as $line)
		parse_line(&$data, $line);
}

function parse_line($data, $line) {
	// get token
	$token = strtok($line, " \t\n\r");

	// skip comments, empty lines
	if (strlen($token) == 0 || $token{0} == "#")
		return;

	// get content after token
	$content = substr($line, 2);

	// parse data
	parse_data(&$data, $token, trim($content));
}

function parse_data($data, $type, $content) {
	switch ($type) {
		case 'v' : $data['verts'][] = parse_vector($content); break;
		case 'vn' : $data['normals'][] = parse_vector($content); break;
		case 'vt' : $data['texcoords'][] = parse_vector($content); break;
		case 'f' : $data['faces'][] = parse_face($content); break;
	}
}

function parse_vector($vector) {
	return explode(" ", $vector);
}

function parse_face($face) {
	$face_components = explode(" ", $face);
	foreach ($face_components as $component) {
		$indexes = explode("/", $component);
		$face_out[] = array_combine(array('v', 'vt', 'vn'), $indexes);
	}
	return $face_out;
}

function pipe_data($data) {
	// face count
	$face_count = pack("L", count($data['faces']));

	// write face count
	fwrite(STDOUT, $face_count);

	// for each face, component count and component data
	foreach ($data['faces'] as $face) {
		$component_count = pack("L", count($face));
		$component_verts = $component_normals = $component_texcoords = '';
		foreach ($face as $component) {
			$component_verts .= pack_vector($data['verts'][$component['v']-1]);
			$component_normals .= pack_vector($data['normals'][$component['vn']-1]);
			$component_texcoords .= pack_vector($data['texcoords'][$component['vt']-1]);
		}
		// write face data
		fwrite(STDOUT, $component_count . $component_verts . $component_normals . $component_texcoords);
	}
}

function pack_vector($vector) {
	$out = '';
	foreach ($vector as $coord)
		$out .= pack("f", sprintf("%.7f", $coord));
	return $out;
}

fwrite(STDERR, "Finished...\n");
fwrite(STDERR, "Faces: " . count($data['faces']) . "\n");
fwrite(STDERR, "Total verts: " . sizeof($data['verts']) . ", normals: " . sizeof($data['normals']) . ", texcoords: " . sizeof($data['texcoords']) . "\n");
?>
