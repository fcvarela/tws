<?
$xml = simplexml_load_file('doc.kml');
$country_count = count($xml->Document->Folder[1]->S_country);

$countries = array();

for ($i=0; $i<$country_count; $i++) {
	// find polygon count
	$has_multi = count($xml->Document->Folder[1]->S_country[$i]->MultiGeometry);
	$name = (string)$xml->Document->Folder[1]->S_country[$i]->name;
	$code = (string)$xml->Document->Folder[1]->S_country[$i]->ISO_2DIGIT;
	
	if ($has_multi) {
		$biggest_str = '';
		$biggest_len = 0;
		$polycount = count($xml->Document->Folder[1]->S_country[$i]->MultiGeometry->Polygon);
		for ($j=0; $j<$polycount; $j++) {
			$coordinates = (string)$xml->Document->Folder[1]->S_country[$i]->MultiGeometry->Polygon[$j]->outerBoundaryIs->LinearRing->coordinates;
			if (strlen($coordinates) > $biggest_len) {
				$biggest_str = $coordinates;
				$biggest_len = strlen($coordinates);
			}
		}
		$boundary = $biggest_str;
	} else
		$boundary = (string)$xml->Document->Folder[1]->S_country[$i]->Polygon->outerBoundaryIs->LinearRing->coordinates;
	
	// add it
	addcountry(&$countries, $name, $code, $boundary);
}

dump_binary_countries(&$countries);

function addcountry($countries, $name, $code, $boundary) {
	$f_boundary = str_replace(",0 ", " ", trim($boundary));
	$f_boundary = substr($f_boundary, 0, -2);
	$a_boundary = explode(" ", $f_boundary);
	foreach ($a_boundary as &$point) {
		$r_point = explode(",", $point);
		$r_point = array_reverse($r_point);
		$point = implode(",", $r_point);
	}
	$f_boundary = implode(" ", $a_boundary);
	$countries[] = array('name' => trim($name), 'code' => trim($code), 'boundary' => $f_boundary);
}

function dump_binary_countries($countries) {
	$count = 0;
	foreach ($countries as &$country) {
		if ($country['code'] == '')
			continue;
		$count++;
		$country_name_len = pack('S', strlen($country['name']));
		fwrite(STDOUT, $country['code']);
		fwrite(STDERR, $country['code']);
		fwrite(STDOUT, $country_name_len);
		fwrite(STDERR, strlen($country['name']));
		fwrite(STDOUT, $country['name']);
		fwrite(STDERR, $country['name']);

		fwrite(STDERR, "\n");
		
		$r_boundary = explode(' ', $country['boundary']);
		fwrite(STDOUT, pack('S', count($r_boundary)));
		foreach ($r_boundary as &$boundary) {
			$r_bound = explode(',', $boundary);
			fwrite(STDOUT, pack('f', $r_bound[0]));
			fwrite(STDOUT, pack('f', $r_bound[1]));
		}
	}
	fwrite(STDERR, "GOT $count countries\n");
}


?>
